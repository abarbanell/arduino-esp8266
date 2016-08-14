#include <ESP8266WiFi.h>
#include "config.h"

const char *ssid = MY_SSID;
const char *passwd = MY_PASSWD;
const char *channel = THINGSPEAK_CHANNEL;
const char *apiKey = THINGSPEAK_WRITE_API_KEY;
const int delayms = 5 * 60 * 1000; // every 5 minutes
const char *server =  "api.thingspeak.com";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(5000);
  connectWifi();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("serial heartbeat - ms since boot: ");
  Serial.print(millis());
  Serial.println();
  sendData(WiFi.hostname(), millis());
  delay(delayms);
  
}

void connectWifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
    // temporary fix until SDK 1.5.4 is used
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
    // temporary fix end
  WiFi.begin(ssid, passwd);
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(WiFi.status());
      Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  
}

void sendData(String hostname, long uptime) {
  WiFiClient client;
  
   if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
       Serial.println("WiFi Client connected ");
       
       String postStr = apiKey;
       postStr += "&field1=";
       postStr += String(hostname);
       postStr += "&field2=";
       postStr += String(uptime);
       
       client.print("POST /update HTTP/1.1\n");
       client.print("Host: api.thingspeak.com\n");
       client.print("Connection: close\n");
       client.print("X-THINGSPEAKAPIKEY: ");
       client.print(apiKey);
       client.print("\n");
       client.print("Content-Type: application/x-www-form-urlencoded\n");
       client.print("Content-Length: ");
       client.print(postStr.length());
       client.print("\n\n");
       client.print(postStr);
       delay(1000);
   
   }//end if
   client.stop();
}

