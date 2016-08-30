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
  int nDev;
    
  Serial.begin(115200); //Opens USB-Serial connection for terminal
  delay(5000);
  Serial.print("Serial interface is ready\r\n");
  connectWifi();
  Wire.begin(4, 5); // sda on pin D2, scl on pin D1
  nDev = i2cscan();
  sendHeartbeat(WiFi.hostname(), millis(), nDev);
  writeI2CRegister8bit(0x20, 6); //reset I2C Soil Sensor
}

void loop() {
  // put your main code here, to run repeatedly:
 
  Serial.print("WiFi heartbeat - ms since boot: ");
  Serial.print(millis());
  Serial.println();
  sendHeartbeat(WiFi.hostname(), millis(), -1);
  sendSoilData(WiFi.hostname());
  delay(delayms);
}

int i2cscan() { 
  byte error, address; 
  int nDevices;
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
  return(nDevices);  
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

void sendSoilData(String hostname) {  
  WiFiClient client;
  int capacitance, temperature, light; 
  float tempC; // in Celsius
  
  capacitance = readI2CRegister16bit(0x20, 0); //read capacitance register
  temperature = readI2CRegister16bit(0x20, 5); //temperature register
  tempC = temperature / 10.0; // convert to Celsius
  writeI2CRegister8bit(0x20, 3); //request light measurement 
  delay(3000); // wait 3 sec before reading
  light = readI2CRegister16bit(0x20, 4); //read light register

  Serial.println("starting to send soil Data: " + hostname + ", " + String(capacitance) + " to server: " + server);
   if (client.connect(server, port)) { 
       Serial.println("WiFi Client connected ");
       
       String postStr = "{ \"host\": \"";
       postStr += String(hostname);
       postStr += "\", \"sensor\": \"";
       postStr += "[capacitance, temperature, light]";
       postStr += "\", \"capacitance\": ";
       postStr += String(capacitance);
       postStr += ", \"temperature\": ";  
       postStr += String(tempC);
       postStr += ", \"light\": ";  
       postStr += String(light);       
       postStr += " }";
       
       String url = "POST /api/collections/sensor?user_key=";
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

  void sendHeartbeat(String hostname, long uptime, int i2cDevices) {
  WiFiClient client;


  Serial.println("starting to sendData: " + hostname + ", " + String(uptime) + " to server: " + server);
   if (client.connect(server, port)) { 
       Serial.println("WiFi Client connected ");
       
       String postStr = "{ \"host\": \"";
       postStr += String(hostname);
       postStr += "\", \"uptime\": \"";
       postStr += String(uptime);
       if (i2cDevices >= 0) {
         postStr += "\", \"i2cDevices\": ";
         postStr += String(i2cDevices);
       }
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

// from https://github.com/Miceuz/i2c-moisture-sensor

void writeI2CRegister8bit(int addr, int value) {
  Wire.beginTransmission(addr);
  Wire.write(value);
  Wire.endTransmission();
}

unsigned int readI2CRegister16bit(int addr, int reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  delay(20);
  Wire.requestFrom(addr, 2);
  unsigned int t = Wire.read() << 8;
  t = t | Wire.read();
  return t;
}



