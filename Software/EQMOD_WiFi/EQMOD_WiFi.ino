// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       EQMOD_WiFi.ino
    Created:	2018-08-06 11:28:57 AM
    Author:     JOHNWIN10PRO\John
*/

// Define User Types below here or use a .h file
//


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>

#define UART_BAUD 9600
#define EQ6_BAUD 9600
#define packetTimeout 20 // ms (if nothing more on UART, then send packet)
#define bufferSize 8192

const char* APNAME = "EQMODWiFi";
const char* APPASSWORD = "Shillit0";

//WiFiManager wifiManager;
WiFiUDP Udp;

const int localUdpPort = 11880;     // local port to listen on
char incomingPacket[255];           // buffer for incoming packets

SoftwareSerial swSerial1(13, 15, false, 256);
uint8_t replyPacket[bufferSize];
uint16_t replyLen = 0;

void setup()
{
	Serial.begin(UART_BAUD); // USB Serial
	//  Serial.swap();
	Serial.flush();
	swSerial1.begin(EQ6_BAUD); // EQ6 Serial
	swSerial1.flush();

	WiFi.mode(WIFI_STA);
	Serial.printf("Connecting to %s ", APNAME); // USB Serial
	WiFi.begin(APNAME, APPASSWORD);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println(" connected"); // USB Serial

	Udp.begin(localUdpPort);
	Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort); // USB Serial
}

void loop()
{
	int packetSize = Udp.parsePacket();
	if (packetSize > 0)
	{
		// receive incoming UDP packets
		Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort()); // USB Serial
		int packetLen = Udp.read(incomingPacket, 255);
		if (packetLen > 0)
		{
			incomingPacket[packetLen] = 0;
		}
		// Send to EQ6
		swSerial1.write(incomingPacket, packetLen);
		//  and copy to console
		Serial.printf("UDP packet contents >>> %d,  %s\n", packetLen, incomingPacket); // USB Serial

		// initialize reply pointer (also length)
		replyLen = 0;
		// Read the EQ6 data until pause
		while (1) {
			if (swSerial1.available()) {
				replyPacket[replyLen] = (char)swSerial1.read(); // read char from EQ6
				if (replyLen < (bufferSize - 1)) replyLen++;
				yield();
			}
			else {
				delay(packetTimeout);
				if (!swSerial1.available()) {
					break;
				}
			}
		}
		replyPacket[replyLen] = 0;

		// send back a reply, to the IP address and port we got the packet from
		if (replyLen > 0) {
			Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
			Udp.write(replyPacket, replyLen);
			Udp.endPacket();
			Serial.printf("Sent  %d bytes >>>  %s to %s, port %d\n", replyLen, replyPacket, Udp.remoteIP().toString().c_str(), Udp.remotePort()); // USB Serial
		}
	}
}
