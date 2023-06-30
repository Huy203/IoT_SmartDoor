#include "connWifi.h"
// #include <HTTPClient.h>
#include <Arduino.h>
#include "DHT.h"

const int dhtPin = 22;
const int dhtType = DHT22; // Type of dht

DHT dht(dhtPin, dhtType);

const String url = "link";

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  conn(); // connect to Wifi Wokwi-Guest
  dht.begin();
}

void loop()
{

  // HTTPClient http;
  // http.begin(url);
  // int status = http.GET();
  // Serial.println(status);

  delay(10); // this speeds up the simulation

  bool checkTemp = false;
  //checkTypeTemp();
  float humid = dht.readHumidity();
  float temp = dht.readTemperature(checkTemp);
  Serial.print("Humidity: ");
  Serial.print(humid);
  Serial.println("%");
  Serial.print("Temperature: ");
  Serial.print(temp);
  if(checkTemp) Serial.println("°F"); //°: Option/Alt +Shift + 8
  else Serial.println("°C"); 
  delay(500);
}
