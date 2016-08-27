// Sketch to connect wino-board to wifi and read out the assigned IP address.
// this will be the basis for more functionality later.
// all the SerialUSB output is just for debugging and not needed if this logic
// is used in other sketches.

#include <ESP8266WiFi.h>
#include <Wire.h>
#include "config.h" 

const int delayms = 5 * 60 * 1000;
const char *server = LG_HOST;
const int port = LG_PORT;


void setup() {
  Serial.begin(115200); //Opens USB-Serial connection for terminal
  delay(5000);
  Serial.print("Serial interface is ready\r\n");
  connectWifi();
  Wire.begin(4, 5); // sda on pin D2, scl on pin D1
}

void loop() {
  // put your main code here, to run repeatedly:
  byte error, address;
  int nDevices;
  
  Serial.print("WiFi heartbeat - ms since boot: ");
  Serial.print(millis());
  Serial.println();
  Serial.println("Scanning i2c devices... ");

  nDevices = 0; 
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4) {
      Serial.print("--Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("--No I2C devices found\n");
  else
    Serial.println("done\n");
 
  sendData(WiFi.hostname(), millis(), nDevices);
  delay(delayms);
  
}

void connectWifi() {
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
    // temporary fix until SDK 1.5.4 is used
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
    // temporary fix end
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(WiFi.status());
      Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected with IP: "),
  Serial.println(WiFi.localIP());
}


void sendData(String hostname, long uptime, int i2cDevices) {
  WiFiClient client;

  Serial.println("starting to sendData: " + hostname + ", " + String(uptime) + " to server: " + server);
   if (client.connect(server, port)) { 
       Serial.println("WiFi Client connected ");
       
       String postStr = "{ \"hostname\": \"";
       postStr += String(hostname);
       postStr += "\", \"uptime\": \"";
       postStr += String(uptime);
       postStr += "\", \"i2cDevices\": ";
       postStr += String(i2cDevices);
       postStr += " }";
       
       String url = "POST /api/collections/hb?user_key=";
       url += LG_API_KEY;
       url += " HTTP/1.1\n";
       url += "Host: ";
       url += LG_HOST;
       url += "\n";
       url += "Connection: close\n";
       url += "Content-Type: application/json\n";
       url += "Content-Length: ";
       url += String(postStr.length());
       url += "\n\n";
       
       Serial.println(url);
       Serial.println(postStr);
       client.print(url);
       client.print(postStr);
       delay(1000);
   
   } else {
     Serial.println("could not connect WifiClient");
   }
   client.stop();
}



