#include "define.h"
#include "connWifi.h"
#include "humidTemp.h"
// #include <HTTPClient.h>
#include <Arduino.h>
#include <string> 

using namespace std;

const int dhtPin = 22;
const int dhtType = DHT22; // Type of dht

const int outsidePirPin = 2; // Pin of 2 PIR sensors
const int insidePirPin = 4;

DHT dht(dhtPin, dhtType);

const String url = "link";

int outsideVal = 0; // read value of 2 PIR sensors
int insideVal = 0;

int outsidePirState = LOW; // state of 2 PIR sensors
int insidePirState = LOW;
int doorState = false; // state of door - false -> closed | true -> opened

void motionDetection(){
  outsideVal = digitalRead(outsidePirPin);
  insideVal = digitalRead(insidePirPin);

  if(outsideVal == HIGH){
    if(outsidePirState == LOW){
      Serial.println("People from outside detected - Door opened");
      doorState = true;
      outsidePirState = HIGH;
    }
  }
  else {
    if(outsidePirState){
      Serial.println("No people from outside");
      outsidePirState = LOW;
    }
  }
  
  if(insideVal == HIGH){
    if(insidePirState == LOW){
      Serial.println("People from inside detected - Door opened");
      doorState = true;
      insidePirState = HIGH;
    }
  }
  else {
    if(insidePirState){
      Serial.println("No people from inside");
      insidePirState = LOW;
    }
  }

  if(outsidePirState == LOW && insidePirState == LOW && doorState){
    Serial.println("No people detected - Door closed");
    doorState = false;
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  conn();   // connect to Wifi Wokwi-Guest
  pinMode(outsidePirPin, INPUT);
  pinMode(insidePirPin, INPUT);
  dht.begin();
}

void loop()
{
  // HTTPClient http;
  // http.begin(url);
  // int status = http.GET();
  // Serial.println(status);

  // This is function Warning
  delay(10); // this speeds up the simulation
  humidity(dht);
  temperature(dht, Cescius);
  delay(1000);

  // Object detection - open/close door
  motionDetection();

  // This is function control
  string response = "ON";
  if (response == "ON")
  {
  }
  else
  {
  }

  
}
