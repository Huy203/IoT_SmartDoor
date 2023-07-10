#include "define.h"
#include "connWifi.h"
#include "humidTemp.h"
// #include <HTTPClient.h>
#include <Arduino.h>

const int dhtPin = 22;
const int dhtType = DHT22; // Type of dht


DHT dht(dhtPin, dhtType);

const String url = "link";

#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(34);
DallasTemperature sensor(&oneWire);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  conn(); // connect to Wifi Wokwi-Guest
  dht.begin();
  sensor.begin();
}

void loop()
{
  
  // HTTPClient http;
  // http.begin(url);
  // int status = http.GET();
  // Serial.println(status);

  delay(10); // this speeds up the simulation
  humidity(dht);
  temperature(dht, Cescius);
  delay(1000);
}
