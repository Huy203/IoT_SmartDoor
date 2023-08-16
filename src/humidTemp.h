#include "DHT.h"
#include <string.h>

// Init DHT
const int dhtPin = 22;
const int dhtType = DHT22; // Type of dht

static DHT dht(dhtPin, dhtType);

float humidity()
{
  float humid = dht.readHumidity();
  Serial.print("Humidity: ");
  Serial.print(humid);
  Serial.println("%");
  return humid;
}

// function to get temperature
float temperature(bool type)
{
  float temp = dht.readTemperature(type);
  Serial.print("Temperature: ");
  Serial.print(temp);
  if (type)
    Serial.println("°F"); // °: Option/Alt +Shift + 8
  else
    Serial.println("°C");
  if (type && temp > 150)
  {
    Serial.println("DANGEROUS");
  }
  if (!type && temp > 60)
  {
    Serial.println("DANGEROUS");
  }
  return temp;
}