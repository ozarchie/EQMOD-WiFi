# Setup

**Not working yet**
*When the EQMOD WiFi adapter starts up, it sets it up in Station mode and tries to connect to a previously saved Access Point. If this is unsuccessful (or if no previous network has been saved) it changes the EQMOD WiFi adapter into Access Point mode and starts up a DNS and WebServer at default IP address “192.168.4.1”.*

*In Access Point mode, the user device (iOS, Android, or Windows) can connect to that WiFi network, using the ssid of “EQMOD” and password of “shillito”. [You can change this in the code]. It will then get its IP address assigned. By pointing the browser to “192.168.4.1”, the user can access a web page that will allow the user to set the users’ own wireless network configuration (ssid, password).*

The WiFi adapter uses the default ssid/pass to connect to a user network. This can be changed from the default in the code.  
Once saved, the EQMOD WiFi adapter will restart and log onto the users’ network in Station mode.  

# SynScanPro

## Introduction 

The SynScanPro application detects the EQMOD WiFi adapter by talking to UDP port 11880. The EQMOD WiFi adapter listens on UDP port 11880 and forwards the received data to the mount serial port at 9600 baud. It listens for the serial (9600 baud) response from the mount and forwards that data to the UDP port that it received the initial data from. An internal virtual serial port at 9600 baud has been established. It is a simple UDP-based IP-to-serial bridge.

The SynScanPro Application needs to be setup. This is done via the Settings screen.
## Windows app notes

1. [Startup](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiWin-1.PNG)  
2. [Settings](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiWin-2.PNG)  
3. [UDP Screen](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiWin-3.PNG)  
4. [Connect](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiWin-4.PNG)  
5. [Connecting](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiWin-5.PNG)  
6. [Connected](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiWin-6.PNG)  
7. [Error Screen](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiWin-7.PNG)  

Select settings from the main screen.

## Android app notes
1. [Startup](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiAndroid-1.png) 
2. [Settings](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiAndroid-2.png) 
3. [Reveal Connection Settings](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiAndroid-3.png) 
4. [UDP Screen](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiAndroid-4.png) 
5. [Connect](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiAndroid-5.png) 
6. [Connecting](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiAndroid-6.png) 
7. [Connected Warning](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiAndroid-7.png) 
8. [Connected](https://github.com/ozarchie/EQMOD-WiFi/blob/master/Documentation/images/EQMODWiFiAndroid-8.png) 

Note2: Tapping the blank line between "Wi-Fi Setting" and "Help" under "settings " eight times. Scroll up and down and it will show extra line "Connect Settings" under "Help"

## Needs confirmation on Apple.
