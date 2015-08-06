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
* built a mini transistor not gate to fix code loading and booting bugs (wouldn't boot because gpio2 was held high by dust monitor)

[This](http://www.esp8266.com/viewtopic.php?f=13&t=1730) explains that gpio0 & 2
both need to be high (or floating) for a normal boot. So with the dust sensor
connected on gpio2, it will often not boot.

## Links

* [sparkfun visualisation with google
 charts](http://phant.io/graphing/google/2014/07/07/graphing-data/)
* [sparkfun thing schematic](https://cdn.sparkfun.com/datasheets/Wireless/WiFi/SparkFun_ESP8266_Thing.pdf)

## Licenses

* Hardware is licensed under the [CERN open hardware license 1.2](http://www.ohwr.org/attachments/2388/cern_ohl_v_1_2.txt).

* Software is licensed under a Creative Commons Attribution 4.0 International License.
