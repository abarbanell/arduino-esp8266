# arduino-esp8266
Software for the esp8266 processor with Arduino IDE

- sketch_uptime_thingspeak: a simple sketch to log the hostname and uptime (in miliseconds) of your esp8266 on the thingspeak platform. Described in more detail [here](http://blog.abarbanell.de/arduino-esp8266/iot/setup/).

- sketch_lg_heartbeat similar to the previous but sending data to [Limitless
Garden IOT backend](https://github.com/abarbanell/limitless-garden)

- sketch_lg_i2cscan running an scan for I2C devioces connected to the esp8266, and adding the count of devices to the hearbeat message sent to Limitless Garden.

- sketch_lg_i2csoil - reading soil humidity (as capacitance), temperature and light from the I2C Soil Moisture sensor and sending to the sensor data collection on Limitless Garden.

See also the project blog at [blog.abarbanell.de/arduino-esp8266](http://blog.abarbanell.de/arduino-esp8266) for more details on each of these.


