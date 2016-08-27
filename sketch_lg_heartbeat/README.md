# sketch_lg_heartbeat

This is an example for the  [ESP8266](http://www.esp8266.com/)
how to connect to the WIFI network and send a REST post to the
[limitless-garden](https://github.com/abarbanell/limitless-garden)
backend.


WIFI passwords and configuration parameters will be injected via a local
file config.h - see below.


# Usage

First you can 

```
$ git clone git@github.com:abarbanell/arduino-esp8266.git
$ cd arduino-esp8266/sketch_lg_heartbeat
```

However, before you compile this sketch you need to create a file
"config.h" with your own config values like this: 

```
/*
 * config variables - this file should not be in source control as it 
 * will show your live credentials.
 */

#ifndef WIFI_SSID
#define WIFI_SSID "MY-SSID"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "MY-WIFI-PASSWORD"
#endif

#ifndef LG_HOST
#define LG_HOST "my.lg.host.com"
#endif

#ifndef LG_PORT
#define LG_PORT 80
#endif

#ifndef LG_API_KEY
#define LG_API_KEY "your-api-key"
#endif

```

Of course you want to have your own values in here. 

Then you can compile and upload the sketch to your esp8266 as usual and
you should see after a while the IP address printed on the serial monitor
and a json message being posted to the backend.


