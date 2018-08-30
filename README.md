# EQMOD-WiFi
An ESP8266 based WiFi to serial, WiFi to UDP, tested on an EQ6 mount

## Introduction
This project describes a WiFi interface used between the EQMOD/SynScanPro software and the DB9 TTL serial interface connector on the Skywatcher EQ6 mount.

## EQMOD - TCP  

The EQMOD interface needs two WeMos ESP8266 devices - one at the host end and one at the mount end. ESP_NOW protocol is used to ensure the connection is fast. The WeMos device firmware uses ESP_NOW to receive and send the data from/to the EQMOD serial port. Inside, it sends and receives at 3.3v level to the EQ6 mount.

On a Windows/Mac/Android/Apple device, you can use the same mount interface in the SynScanPro app. By selecting UDP mode, the mount will work in either access point (mount is an AP) or station (mount joins the local network) modes.  
UDP mode and AP/STA modes are selected by jumpers/switches on the mount interface.  
In station mode, the ssid and password needs to be changed in the firmware. Later revisions may include the ability to do this OTA.  

## SynScanPro - UDP  

The SynScanPro interface uses UDP to send and receive data to and from the WeMos device. The WeMos device sends and receives the data at 3v3 level to the EQ6 mount.  

It has only been tested on the EQ6 mount.    

## Notices
This interface uses sample code from the ESP8266 arduino repositories.  
There are numerous HowTo videos and support groups.    
**Some WeMos devices have problems downloading. Search the discussion groups first.**   

*I use genuine WeMos devices and have not experienced the reported issues, which presumably come from copycat devices. I also use Visual Studio for arduino development which may use different download software and interfaces.* 

Below are the Fritzing schematics and breadboard. Take care as I have not verified it, but it follows my prototype.  

![Schematic](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFi_schem.png) 
![Breadboard](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFi_bb.png) 
