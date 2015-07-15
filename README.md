# Dust Sensor

* Measures dust levels in Bristol Hackspace's messy space
* Logs data to Sparkfun's data service
* [Visualised](http://bristol.hackspace.org.uk/dust-sensor/html/graph.html) using google chart

## BOM

* [grove dust sensor](http://www.seeedstudio.com/wiki/Grove_-_Dust_Sensor) -_
* ESP8266

## Fixed, but need to update schematic/pcb

* add cap between DTR & reset for programming
* caps on power rail and decoupling on ESP
* change sensor pin to gpio2 as when connected to gpio0 forces programming mode

## Bugs

* why does sensor plugged in stop upload of code working?
* sometimes won't boot unless sensor is unplugged (PSU quality issue)

## Links

* [sparkfun visualisation with google
 charts](http://phant.io/graphing/google/2014/07/07/graphing-data/)
* [sparkfun thing schematic](https://cdn.sparkfun.com/datasheets/Wireless/WiFi/SparkFun_ESP8266_Thing.pdf)
