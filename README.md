# TrackerGPSRF #

## Overview:
TrackerGPSRF is tracker GPS transmitter through RF 433 Mhz up to 1.8Km.
It composed by two module:
- the tracker GPS/RF which had be to attach to the object which has to be track.
- the Hotspot/Webserver which create a hotspot wifi and hosted webserver to track the object in realtime

## Component to buy and assemble:

### Module GPS/RF:
- GPS board: 1 x NEO6MV2
- RF board: 1 x HC12
- 1 lipo 1S 3.7V 150mah(more than 2h of uptime) to 450mah (or more depend on uptime needed and the weight that can be use for application).

### Module Hotspot/Webserver:
- Arduino board: 1 x nodemcu v2 (or sparkfun or wemos...)
- RF board: 1 x HC12
- 1 lipo 1S 3.7V 450mah mini to 1200mah (or more depend on uptime needed and the weight that can be use for application) or USB powerbank or USB charger 

## How it work:
To be update.

## How to parameter HC12 to increase from 1Km to 1.8Km:
Before wired the two HC12 Board you can configure them to increase their range to 1.8km. 
For that you have to use an USB TTL and send AT command(like describe in datasheet) to pass the board to 2400 or 1200 baud and FU4 mode.
in this configuration the HC12 can transmit only 60 bits by message... (this fonctionnality had not been tested for the moment).

_Default parameter is 9600 baud and FU3 mode_.

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
You have choice to edit source and build your own firmware using Arduino IDE
https://www.arduino.cc/en/Main/software
and configure the ESP8266 library
https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/installing-the-esp8266-arduino-addon

Or you can use the PreBuild Firmware push directly in the git (PreBuild_Firmware directory) with the flasher below:
https://nodemcu.readthedocs.io/en/latest/en/flash/#nodemcu-flasher

## How to access to hotspot and webserver: 
you add to search the wifi connection behind ans use the password (it can be change in source)
###### WiFi HotSpot SSID: _TrackerGPSRF_
###### WiFi HotSpot PASSWORD: _123456789_

Once you are connected open a web page on http://192.168.4.1/ that's all
