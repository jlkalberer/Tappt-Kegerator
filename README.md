Tappt-Kegerator Server
===============

The Arduino backed kegerator code for hooking up to the Tappt service.

This communicates with a website in order to track pours, beer left in the keg, and temperature.

This is the list of parts:

* [Arduino UNO Rev3](http://www.seeedstudio.com/depot/arduino-uno-rev3-p-694.html)
* [Wifi Shield](http://www.seeedstudio.com/depot/wifi-shield-p-1220.html) - This is a different shield that the one found on Sparkfun which is currently incompatible with the Sparkfun WiFly library.
* [NFC Shield V2.0](http://www.seeedstudio.com/depot/nfc-shield-v20-p-1370.html)
* [One Wire Digital Temperature Sensor - DS18B20](https://www.sparkfun.com/products/245)
* [SwissFlow SF800 - LOW PRESSURE FLOW METER](http://www.swissflow.com/sf800.html) - NSF certified beverage grade sensor.  Since it is infrared we shouldn't have issue with foamy beer.
* [NSF-Certified Acetal Solenoid Valve, NC, 3/8 NPT Female, 1/4" Quick Disconnect, 12 VDC](http://www.mcmaster.com/nav/enter.asp?partnum=7876K28)
* [JOHN GUEST MALE CONNECTOR BSPT - 3/8 X 1/4 BSPT](http://www.freshwatersystems.com/p-2341-male-connector-bspt-38-x-14-bspt.aspx)
* [JOHN GUEST FEMALE ADAPTER NPTF - 3/8 X 1/4 NPTF](http://www.freshwatersystems.com/p-2508-female-adapter-nptf-38-x-14-nptf.aspx)
* [4.7k pull-up resistor](http://www.basicmicro.com/Resistor-47K-Ohm-14-Watt_p_94.html) - Usually buy these in a kit as it's a lot cheaper than this site.