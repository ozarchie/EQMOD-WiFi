# EQMOD-WiFi
An ESP8266 based WiFi to serial, tested on an EQ6 mount

## Introduction
This project describes a WeMos WiFi interface used between the EQMOD/SynScanPro software and the DB9 TTL serial interface connector on the Skywatcher EQ6 mount.

## EQMOD - TCP  

The EQMOD interface uses [HWVirtSerial](https://www.hw-group.com/software/hw-vsp3-virtual-serial-port) as a TCP based virtual serial port. The WeMos device firmware uses TCP to receive and send the data from/to the EQMOD virtual serial port. Inside, it sends and receives at 3.3v level to the EQ6 mount. On a Windows machine, you can use the same virtual serial interface in the SynScanPro app. But, this does not work for the Android/Apple apps.  

## SynScanPro - UDP  

The SynScanPro interface uses UDP to send and receive data to and from the WeMos device. The WeMos device then sends and receives the data at 3v3 level to the EQ6 mount.  

You need to load different firmware for SynScanPro and the EQMOD interfaces - or have two devices?  

It has only been tested on the EQ6 mount.  

*TODO: 
Find a way to support both TCP and UDP in the WeMos firmware.  
Put in 3v3 <> TTL buffers  *  

## Notices
This interface uses sample code from the ESP8266 arduino repositories.  
There are numerous HowTo videos and support groups.    
**Some WeMos devices have problems downloading. Search the discussion groups first.**   

*I use genuine WeMos devices and have not experienced the reported issues, which presumably come from copycat devices. I also use Visual Studio for arduino development which may use different download software and interfaces.* 

Below are the Fritzing schematics and breadboard. Take care as I have not verified it, but it follows my prototype.  

![Schematic](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFi_schem.png) 
![Breadboard](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFi_bb.png) 
