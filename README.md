# TrackerGPSRF #

## Description:

## How to wire GPS/RF module:
![alt text](https://github.com/Hellorheaven/TrackerGPSRF/blob/master/GPSRF_module.png)

## How to wire Hotspot/Webserver module:

![alt text](https://github.com/Hellorheaven/TrackerGPSRF/blob/master/Webserver_module.png)

## How to power each module:
Just put a lipo 1S 3.7v (or other battery that give 3.7V)
- the + of the battery on VCC(for gps module) or VIN(for Hotspot/Webserver module)
- the - of the battery on GND(-)

You can also power the Hotspot/Webserver(nodemcu) with USB.

## How to flash firmware on the Hotspot/Webserver module
https://nodemcu.readthedocs.io/en/latest/en/flash/#nodemcu-flasher

## How to access to hotspot and webserver: 
you add to search the wifi connection behind ans use the password (it can change in source)
###### WiFi HotSpot SSID: _GPSRFWebserver_
###### WiFi HotSpot PASSWORD: _RFGPS123456789_

Once you are connected open a web page on http://192.168.4.1/ that's all
