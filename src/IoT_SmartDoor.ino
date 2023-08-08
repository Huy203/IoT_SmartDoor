#include "define.h"
#include "connWifi.h"
#include "humidTemp.h"
#include <PubSubClient.h>
#include <Arduino.h>
#include <string> 

using namespace std;

const int dhtPin = 22;
const int dhtType = DHT22; // Type of dht

const int outsidePirPin = 2; // Pin of 2 PIR sensors
const int insidePirPin = 4;

DHT dht(dhtPin, dhtType);


//***Set server***
const char* mqttServer = "broker.hivemq.com"; 
int port = 1883;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

TypeTemp type = Cescius;

void mqttConnect() {
  while(!mqttClient.connected()) {
    Serial.println("Attemping MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if(mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");

      //***Subscribe all topic you need***
     
    }
    else {
      Serial.println("Try again in 2 seconds");
      delay(2000);
    }
  }
}

//MQTT Receiver
void callback(char* topic, byte* message, unsigned int length) {
  Serial.println(topic);
  String strMsg;
  for(int i=0; i<length; i++) {
    strMsg += (char)message[i];
  }
  Serial.println(strMsg);

  //***Code here to process the received package***

}


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
  mqttClient.setServer(mqttServer, port);
  mqttClient.setCallback(callback);
  mqttClient.setKeepAlive( 90 );
}

void loop()
{
  delay(10); // this speeds up the simulation
  if(!mqttClient.connected()) {
    mqttConnect();
  }
  mqttClient.loop();

  /*Function WARNING*/ 

  float humid = humidity(dht);
  float temp = temperature(dht, type);
  delay(500);

  char buffer [50];

  //Publish humidity
  snprintf(buffer, 50, "%.2lf", humid);
  Serial.print(buffer);
  mqttClient.publish("IOT_SMARTDOOR/humid", buffer);
  //Publish temperature
  snprintf(buffer, 50, "%.2lf", temp);
  Serial.print(buffer);
  mqttClient.publish("IOT_SMARTDOOR/temp", buffer);
  //Publish type of temperature
  snprintf(buffer, 50, "%s", type ? "true":"false");
  Serial.print(buffer);
  mqttClient.publish("IOT_SMARTDOOR/type_temp", buffer);
  
   // Object detection - open/close door
  motionDetection();

  /*Function CONTROL*/
  

}
