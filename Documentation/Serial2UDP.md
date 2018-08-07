# Setup

When your EQMOD WiFi adapter starts up, it sets it up in Station mode and tries to connect to a previously saved Access Point. If this is unsuccessful (or if no previous network has been saved) it changes the EQMOD WiFi adapter into Access Point mode and starts up a DNS and WebServer at default IP address “192.168.4.1”.  
In Access Point mode, the user device (iOS, Android, or Windows) can connect to that WiFi network, using the ssid of “EQMOD” and password of “shillito”. It will then get its IP address assigned. By pointing the browser to “192.168.4.1”, the user can access a web page that will allow the user to set the users’ own wireless network configuration (ssid, password).  
Once saved, the EQMOD WiFi adapter will restart and log onto the users’ network in Station mode.  

# SynScanPro

## Introduction 

The SynScanPro application detects the EQMOD WiFi adapter by talking to UDP port 11880. The EQMOD WiFi adapter listens on UDP port 11880 and forwards the received data to the mount serial port at 9600 baud. It listens for the serial (9600 baud) response from the mount and forwards that data to the UDP port that it received the initial data from. An internal virtual serial port at 9600 baud has been established. It is a simple UDP-based IP-to-serial bridge.

The SynScanPro Application needs to be setup. This is done via the Settings screen.
## Windows app notes

1. [StartUp](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiAndroid-1.png)   
2. [Connect Settings](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiAndroid-2.png)  
[UDP Screen](github.com/ozarchie/EQMOD-WiFi/tree/master/Documentation/images/EQMODWiFiWin-3.png)  
[Connect](github.com/ozarchie/EQMOD-WiFi/tree/master/Documentation/images/EQMODWiFiWin-4.png)  
[Connecting](github.com/ozarchie/EQMOD-WiFi/tree/master/Documentation/images/EQMODWiFiWin-5.png)  
[Connected](github.com/ozarchie/EQMOD-WiFi/tree/images/master/Documentation/EQMODWiFiWin-6.png)  
[Error Screen](github.com/ozarchie/EQMOD-WiFi/tree/images/master/Documentation/EQMODWiFiWin-7.png)  

Select settings from the main screen.

## Android app notes
[alt text](github.com/ozarchie/EQMOD-WiFi/tree/master/Documentation/images/EQMODWiFiAndroid-1.png "StartUp")
[alt text](github.com/ozarchie/EQMOD-WiFi/tree/master/Documentation/images/EQMODWiFiAndroid-2.png "Settings")
Tapping the blank line between "Wi-Fi Setting" and "Help" under "settings " 8 times  
Scroll up and down and it will show extra line "Connect Settings" under "Help"  
[alt text](github.com/ozarchie/EQMOD-WiFi/tree/master/Documentation/images/EQMODWiFiAndroid-3.png "Reveal Settings")
[alt text](github.com/ozarchie/EQMOD-WiFi/tree/master/Documentation/images/EQMODWiFiAndroid-4.png "UDP Screen")
[alt text](github.com/ozarchie/EQMOD-WiFi/tree/master/Documentation/images/EQMODWiFiAndroid-5.png "Connect")
[alt text](github.com/ozarchie/EQMOD-WiFi/tree/master/Documentation/images/EQMODWiFiAndroid-6.png "Connecting")
![alt text](github.com/ozarchie/EQMOD-WiFi/master/Documentation/tree/images/EQMODWiFiAndroid-7.png "Connected Warning")
![alt text](github.com/ozarchie/EQMOD-WiFi/master/Documentation/tree/images/EQMODWiFiAndroid-8.png "Connected")



## Needs confirmation on Apple.
