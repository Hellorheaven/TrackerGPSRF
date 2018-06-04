# TrackerGPSRF 
<br />
## Description: 
<br />
## How to wire GPS/RF module: 
![alt text](https://github.com/Hellorheaven/TrackerGPSRF/blob/master/GPSRF_module.png)<br />
## How to wire Hotspot/Webserver module:
![alt text](https://github.com/Hellorheaven/TrackerGPSRF/blob/master/Webserver_module.png)<br />
<br />
## How to power each module: 
Just put a lipo 1S 3.7v (or other battery that give 3.7V) <br />
- the + of the battery on VCC(for gps module) or VIN(for Hotspot/Webserver module) <br />
- the - of the battery on GND(-)<br />
<br />
You can also power the Hotspot/Webserver(nodemcu) with USB.<br />
<br />
## How to flash firmware on the Hotspot/Webserver module
https://nodemcu.readthedocs.io/en/latest/en/flash/#nodemcu-flasher<br />
<br />
## How to access to hotspot and webserver: 
you add to search the wifi connection behind ans use the password (it can change in source)<br />
<br />
###### WiFi HotSpot ssid : GPSRFWebserver 
###### WiFi HotSpot password : RFGPS123456789 
<br />
Once you are connected open a web page on http://192.168.4.1/ that's all<br />
