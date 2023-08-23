#include "DHT.h"
#include "define.h"

// Init DHT
const int dhtPin = 22;
const int dhtType = DHT22; // Type of dht

static DHT dht(dhtPin, dhtType);


float humidity()
{
  float humid = dht.readHumidity();
  return humid;
}

float temperature(TypeTemp type)
{
  float temp = dht.readTemperature(type);
  return temp;
}