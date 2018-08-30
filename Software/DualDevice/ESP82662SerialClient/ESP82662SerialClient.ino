// ESP2SerialMaster.ino
// https://esp-idf.readthedocs.io/en/latest/api-reference/wifi/esp_now.html

#include <ESP8266WiFi.h>
#include <TimeLib.h>              //https://github.com/PaulStoffregen/Time
#include <TimeAlarms.h>           //https://github.com/PaulStoffregen/TimeAlarms
#include "myesp_now.h"

extern "C" {
#include <espnow.h>
#include <user_interface.h>
#include "myesp_now.h"
}

uint8_t mountMAC[] = { 0x5C, 0xCF, 0x7F, 0x88, 0x88, 0x88 };
#define WIFI_CHANNEL 4

struct __attribute__((packed)) DataStruct {
	char text[ESP_NOW_MAX_DATA_LEN-1];
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

void initVariant() {
  WiFi.mode(WIFI_AP);
  wifi_set_macaddr(SOFTAP_IF, &mountMAC[0]);
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

  waitingForReply = false;
  memcpy(&recvWiFi.text, incomingData, len);
  recvWiFi.text[len] = 0;         // Null terminate the string
  recvWiFi.len = len;
  waitingForReply = false;
    
  if (Connected) {
    EQxSerial.print(recvWiFi.text);
    dbgSerial.print(recvWiFi.text);
  }

  if (!Connected) {
    Connected = true;
  }
}

// Send data to ESP_NOW
void sendData(uint8_t *macAddr) {
// Indicating outgoing data (either mount serial data, or connection response)
    BlinkmS = FastLEDmS;

// Send data, if not waiting for previous send to complete
if (!dataSending) {
    memcpy(sendWiFi8, &sendWiFi, sendWiFi.len);
    esp_now_send(NULL, sendWiFi8, sendWiFi.len);      // NULL means send to all peers
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

void getEQxSerialData(void) {
  uint8_t n;
  LastmS = millis();
  RxTimeout = 0;
  RxD = 0;
  n = 0;
  while ((RxTimeout < EQxTimeout) && (RxD != 0x0d)) {  // && (RxD != 0x0d)
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
  sendWiFi.len = n;
}

void blinkLed() {
    digitalWrite(LED, !digitalRead(LED));
    if (SavedBlinkmS != BlinkmS) {
      Alarm.timerRepeat(BlinkmS, blinkLed);
      SavedBlinkmS = BlinkmS;
    }
}


//===============================================================================

void setup() {
  
  pinMode(LED, OUTPUT);             // LED indicator
  pinMode(PROTOCOL, INPUT_PULLUP);  // ESP_NOW(1),  UDP(0)
  pinMode(MODE, INPUT_PULLUP);      // AP(1),       STA(0)

  EQxSerial.begin(EQMOD_BAUD);      // Pins Tx(01), Rx(03) (USB)
  dbgSerial.begin(DEBUG_BAUD);      // Pin  D4( 2)

	WiFi.mode(WIFI_STA);              // Station mode
	WiFi.disconnect();
  InitESPNow();

	esp_now_set_self_role(ESP_NOW_ROLE_COMBO);	esp_now_register_send_cb(sendCallBack);					// Sending data to mount
	esp_now_register_recv_cb(recvCallBack);					// Receiving data from mount
	esp_now_add_peer(mountMAC, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);	                    // Need to query whether the mount exists

	dbgSerial.println("Waiting for EDQMOD mount");
	strcpy(sendWiFi.text, "Mount, please reply");	// For loop()
	sendWiFi.len = sizeof("Mount, please reply");
  sendWiFi.len = 0;
  recvWiFi.len = 0;
  Alarm.timerRepeat(BlinkmS, blinkLed);
}

//==============

void loop() {
	if ((!Connected) && (!dataSending)) {
		if (!waitingForReply)			
			sendData(mountMAC);
		WiFiTimeout = millis();
		while (((millis() - WiFiTimeout) < RxTimeout) && dataSending);
		if (dataSending) {
      strcpy(sendWiFi.text, "Mount, please reply"); // For loop()
      sendWiFi.len = sizeof("Mount, please reply");
			dataSending = false;
			Connected = false;
		}
	}
	else {									          // Connected, so wait for Host data
		if (EQxSerial.available())
			getEQxSerialData();
		if (sendWiFi.len > 0)			      //  and send, if any
			sendData(mountMAC);
	}
}


