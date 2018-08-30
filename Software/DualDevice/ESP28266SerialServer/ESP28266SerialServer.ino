// ESP2SerialServer.ino
// https://esp-idf.readthedocs.io/en/latest/api-reference/wifi/esp_now.html

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>              //https://github.com/PaulStoffregen/Time
#include <TimeAlarms.h>           //https://github.com/PaulStoffregen/TimeAlarms
#include "myesp_now.h"

extern "C" {
#include <espnow.h>
#include <user_interface.h>
}

uint8_t smac[] = { 0x5C, 0xCF, 0x7F, 0x88, 0x88, 0x88 };		// Hopefully :) Unique Espressif mac
uint8_t mmac[] = { 0x5C, 0xCF, 0x7F, 0x00, 0x00, 0x00 };		// Master mac address
const uint8_t WIFI_CHANNEL = 4;

char *ssid = "EQMODWiFi";
char *pass = "CShillit0";
IPAddress ip(192, 168, 88, 1);
IPAddress netmask(255, 255, 255, 0);

WiFiUDP udp;
IPAddress remoteIp;
const int localUdpPort = 11880;

struct __attribute__((packed)) DataStruct {
    char text[255];
	uint8_t len;
};

DataStruct sendWiFi;
DataStruct recvWiFi;
uint8_t sendWiFi8[sizeof(sendWiFi)];
uint8_t recvWiFi8[sizeof(recvWiFi)];

#define EQMOD_BAUD 9600
#define DEBUG_BAUD 9600
#define EQxTimeout 10
#define EQxSize ESP_NOW_MAX_DATA_LEN-1

#define dbgSerial Serial1
#define EQxSerial Serial
#define PROTOCOL 5       // GPIO5
#define MODE 4           // GPIO4
#define LED 14           // GPIO14

uint8_t RxD;
uint8_t TxD;

unsigned long RxTimeout;
unsigned long WiFiTimeout;
unsigned long LastmS;
unsigned long CheckmS = 1000;
bool Connected = false;
bool dataSending = false;
bool waitingForReply = false;

unsigned long TxDuS;
unsigned long AckuS;
unsigned long RxDuS;

unsigned long LastLEDmS;
unsigned long FastLEDmS = 200;
unsigned long SlowLEDmS = 800;
unsigned long BlinkmS = SlowLEDmS;
unsigned long SavedBlinkmS = SlowLEDmS;

bool UDPFlag = false;
bool APFlag = false;

void initVariant() {
  WiFi.mode(WIFI_AP);
  wifi_set_macaddr(SOFTAP_IF, &smac[0]);
}

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    dbgSerial.println("ESPNow Init Success");
  }
  else {
    dbgSerial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// Receive data from ESP_NOW
void recvCallBack(uint8_t *senderMac, uint8_t *incomingData, uint8_t len) {
// Indicating incoming data
  BlinkmS = FastLEDmS;
  
// Get local copy
  memcpy(&recvWiFi.text, incomingData, len);      // Receive data from EQMOD Tx
  recvWiFi.text[len] = 0;                         // Null terminate
  recvWiFi.len = len;
  waitingForReply = false;
  
// Check if serial requesting restart
  if ((len == 20) && ((strncmp(recvWiFi.text, "Mount, please reply", 15) == 0))) {
    // Data should not be copied to mount
    recvWiFi.len = 0;
    Connected = false;
    dataSending = false;
    // Reply to reconnect request
    strcpy(sendWiFi.text, "EQMOD Mount V1.0\n");
    sendWiFi.len = sizeof("EQMOD Mount V1.0\n");
    dbgSerial.println("Reconnecting");
    sendData(senderMac);       // Copy data to EQMOD Rx
    return;
  }
  if (!Connected) {               // Capture the serial mac address
    for (byte n = 0; n < 6; n++) {
      mmac[n] = senderMac[n];
    }
  }
}

// Send data to ESP_NOW
void sendData(uint8_t *macAddr) {
// Indicating outgoing data (either mount serial data, or connection response)
    BlinkmS = FastLEDmS;
// If first time, create a peer2peer connection with the sender mac address
  if (!Connected) {
    esp_now_add_peer(macAddr, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0); // Only one paired device - the first one to respond
  }
  
// Send data, if not waiting for previous send to complete
  if (!dataSending) {
    memcpy(sendWiFi8, &sendWiFi, sendWiFi.len);     // Need to satisfy esp_now
    esp_now_send(NULL, sendWiFi8, sendWiFi.len);    // NULL means send to all peers
    dataSending = true;
  }  
}

// Get Send data status
void sendCallBack(uint8_t* mac, uint8_t sendStatus) {
  if (sendStatus == 0) {
    sendWiFi.len = 0;         // Data successfully sent
    BlinkmS = SlowLEDmS;      // Data complete, so back to slow blink
    Connected = true;
    dataSending = false;
    waitingForReply = true;
  }
  else {
    BlinkmS = FastLEDmS;    // Still waiting
  }
}

// Send received WiFi (UDP/ESP_NOW) data to the mount serial data
// Data in recvWiFi.text, length recvWiFi.len
void putMountSerialData(void) {
  uint8_t TxD, n;
  n = 0;
  while (n < recvWiFi.len) {
    TxD = recvWiFi.text[n];
    EQxSerial.write(TxD);
    dbgSerial.write(TxD);
    n += 1;
  }
  recvWiFi.len = 0;   // All data sent
  BlinkmS = SlowLEDmS;
}

// Get mount serial data to send to WiFi (UDP/NOW)
// Data in sendWiFi.text, length sendWiFi.len
void getMountSerialData(void) {
  uint8_t RxD, n;
// Indicate outgoing mount data
  BlinkmS = FastLEDmS;

// Receive mount serial data, with timeout
  LastmS = millis();
  RxTimeout = 0;
  RxD = 0;
  n = 0;
  sendWiFi.text[0] = 0;
  
  // Wait for '=' or '!'
  while ((RxTimeout < EQxTimeout) && (RxD != '=') && (RxD != '!')) {
    RxTimeout = millis() - LastmS;
    if (EQxSerial.available()) {
      LastmS = millis();
      RxD = EQxSerial.read();
    }
  }
  if (RxTimeout < EQxTimeout) {
    sendWiFi.text[n] = RxD;
    dbgSerial.write(RxD);
    n += 1;
  }
  LastmS = millis();
  RxTimeout = 0;  
  // Continue reading until CR
  while ((RxTimeout < EQxTimeout) && (RxD != 0x0d)) {
    RxTimeout = millis() - LastmS;
    if (EQxSerial.available()) {
      LastmS = millis();
      RxD = EQxSerial.read();
      dbgSerial.write(RxD);
      sendWiFi.text[n] = RxD;
      n += 1;
    }
  }
  sendWiFi.text[n] = 0;
  sendWiFi.len = n;     // Length of Rx data
}

void blinkLed() {
    digitalWrite(LED, !digitalRead(LED));
    if (SavedBlinkmS != BlinkmS) {
      Alarm.timerRepeat(BlinkmS, blinkLed);
      SavedBlinkmS = BlinkmS;
    }
}

// ================================================================================================

void setup() {

  pinMode(LED, OUTPUT);             // LED indicator
  pinMode(PROTOCOL, INPUT_PULLUP);  // ESP_NOW(1),  UDP(0)
  pinMode(MODE, INPUT_PULLUP);      // AP(1),       STA(0)
  
  EQxSerial.begin(EQMOD_BAUD);      // Pins Tx(01), Rx(03) (USB)
  EQxSerial.swap();                 // Pins D8(15), D7(13) (TTL)
  dbgSerial.begin(DEBUG_BAUD);      // Pin  D4( 2)
  
  dbgSerial.println("");
  dbgSerial.print("digitalRead(PROTOCOL): ");
  dbgSerial.println(digitalRead(PROTOCOL));
  dbgSerial.print("digitalRead(MODE)    : ");
  dbgSerial.println(digitalRead(MODE));
  
  if (digitalRead(PROTOCOL))
    UDPFlag = false;
  else
    UDPFlag = true;

  if (digitalRead(MODE))
    APFlag = true;
  else
    APFlag = false;
    
  if (APFlag) { 
  // AP mode       device connects directly to EQMODWiFi) (no router)
  // For AP mode:  UDP2Serial: This ESP assigns IP addresses
  // For AP mode:  ESP IP is always 192.168.88.1 (set above)
    dbgSerial.println("Access Point Mode");
    strcpy(ssid, "EQMODWiFi");          // Default EQMOD
    strcpy(pass, "CShillit0");
  	WiFi.mode(WIFI_AP);
  	WiFi.softAPConfig(ip, ip, netmask); // softAP ip
  	WiFi.softAP(ssid, pass);            // softAP ssid, pass
  }
  else {
  // STA mode       EQMODWiFi connects to network router and gets an IP
  // For STA mode:  Host software must detect that IP
  // For STA mode:  UDP2Serial router network assigns IP address
    dbgSerial.println("Station Mode");
    strcpy(ssid, "HAPInet");      // "YourSSID";
    strcpy(pass, "HAPIconnect");  // "YourPass";
  	WiFi.mode(WIFI_STA);
  	WiFi.begin(ssid, pass);
  	while (WiFi.status() != WL_CONNECTED) {
  		delay(100);
      dbgSerial.print(".");
  	}
    dbgSerial.println(" connected");
  }
  
  if (!UDPFlag) {
  // ESP_NOW mode   (EQMODWiFi responds to MAC protocol)
    dbgSerial.println("ESP_NOW Mode");
    InitESPNow();
    dbgSerial.println("ESPNOW2SerialServer");
    dbgSerial.print("Mount soft  mac: "); dbgSerial.println(WiFi.softAPmacAddress());
  	dbgSerial.print("Mount hard  mac: "); dbgSerial.println(WiFi.macAddress());
  	esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  	esp_now_register_recv_cb(recvCallBack);	
  	esp_now_register_send_cb(sendCallBack);
  }
  else {
    dbgSerial.println("UDP Mode");
    udp.begin(localUdpPort);
    dbgSerial.println("UDP2SerialServer");
    if (APFlag) 
      dbgSerial.printf("Now listening at IP %s, UDP port %d\n", WiFi.softAPIP().toString().c_str(), localUdpPort);
    else
      dbgSerial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  }
	sendWiFi.len = 0;
  recvWiFi.len = 0;
  Alarm.timerRepeat(BlinkmS, blinkLed);
}

// -------------------------------------------------------------------------------------------

void loop() {
  if (UDPFlag) {
    recvWiFi.len = udp.parsePacket();
    if (recvWiFi.len > 0) {
      // receive incoming UDP packets
      recvWiFi.len = udp.read(recvWiFi.text, 250);
      if (recvWiFi.len > 0) {
        recvWiFi.text[recvWiFi.len] = 0;
//        dbgSerial.print("RxUDP - len: ");
//        dbgSerial.print(recvWiFi.len);
//        dbgSerial.print(", data: ");
//        dbgSerial.println(recvWiFi.text);
      }
      // Send data to mount
      putMountSerialData();
      
      // Get data from mount
      getMountSerialData();
  
      // send back a reply, to the IP address and port we got the packet from
      if (sendWiFi.len > 0) {
//        dbgSerial.print("TxUDP - len: ");
//        dbgSerial.print(sendWiFi.len);
//        dbgSerial.print(", data: ");
//        dbgSerial.println(sendWiFi.text);
        udp.beginPacket(udp.remoteIP(), udp.remotePort());
        udp.write(sendWiFi.text, sendWiFi.len);
        udp.endPacket();
      }
      sendWiFi.len = 0;
    }    
  }
  else {
  	if (Connected) {
  		if (EQxSerial.available() > 0) 
  		  getMountSerialData();			// Receive data from Mount Tx
  		if (sendWiFi.len > 0)
  			sendData(mmac);				// Copy data to EQMOD Rx
  		if (recvWiFi.len > 0)		// Received data from EQMOD Tx
  			putMountSerialData();		// Copy data to Mount Rx
  	}
  }
}


