// #include "define.h"
#include "connWifi.h"
// #include "humidTemp.h"
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <Arduino.h>
#include <string> 

using namespace std;

// set device pin
// const int dhtPin = 22;

const int outsidePirPin = 25; 
const int insidePirPin = 5;

const int outsideServoPin = 32;
const int insideServoPin = 19;

// // set device
// const int dhtType = DHT22; 
// DHT dht(dhtPin, dhtType);

Servo outsideDoor;
Servo insideDoor;

// set global variable
int outsideVal = 0; // read value of 2 PIR sensors
int insideVal = 0;

int outsidePirState = LOW; // state of 2 PIR sensors
int insidePirState = LOW;
int doorState = false; // state of door - false -> closed | true -> opened

int getIn = 0;
int getOut = 0;

bool onSecurity = false;

// set publish and subcribe topic

const char* controlTopic = "IOT_SMARTDOOR/DoorControl";
const char* getInTopic = "IOT_SMARTDOOR/DoorMonitor/GetIn";
const char* getOutTopic = "IOT_SMARTDOOR/DoorMonitor/GetOut";

//***Set server***
const char* mqttServer = "broker.hivemq.com"; 
int port = 1883;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// TypeTemp type = Cescius;

void mqttConnect() {
  while(!mqttClient.connected()) {
    Serial.println("Attemping MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if(mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");

      //***Subscribe all topic you need***
      mqttClient.subscribe(controlTopic);
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

  if(strcmp(topic, controlTopic) == 0){
    if(strMsg == "SecurityOn"){
      onSecurity = true;
    }
    else if(strMsg == "SecurityOff"){
      onSecurity = false;
    }
    else if(strMsg == "Open"){
      insideDoor.write(0);
      outsideDoor.write(0);
    }
    else if(strMsg == "Close"){
      insideDoor.write(90);
      outsideDoor.write(90);
    }
  }
}

void motionDetection(){
  outsideVal = digitalRead(outsidePirPin);
  insideVal = digitalRead(insidePirPin);

  if(outsideVal == HIGH){
    if(outsidePirState == LOW){
      Serial.println("People from outside detected");
      outsidePirState = HIGH;
      if(!onSecurity){
        doorState = true;
        insideDoor.write(0);
        outsideDoor.write(0);
        if(insideVal == HIGH) 
          getOut++;
      }
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
      Serial.println("People from inside detected");
      insidePirState = HIGH;
      if(!onSecurity){
        doorState = true;
        insideDoor.write(0);
        outsideDoor.write(0);
        if(outsideVal == HIGH)
          getIn++;
      }
    }
  }
  else {
    if(insidePirState){
      Serial.println("No people from inside");
      insidePirState = LOW;
    }
  }

  if(outsidePirState == LOW && insidePirState == LOW && doorState){
    Serial.println("No people detected");
    insideDoor.write(90);
    outsideDoor.write(90);
    doorState = false;
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  conn();   // connect to Wifi Wokwi-Guest

  mqttClient.setServer(mqttServer, port);
  mqttClient.setCallback(callback);
  mqttClient.setKeepAlive(90);

  insideDoor.attach(insideServoPin, 500, 2400);
  outsideDoor.attach(outsideServoPin, 500, 2400);

  pinMode(outsidePirPin, INPUT);
  pinMode(insidePirPin, INPUT);
  // dht.begin();
}

void loop()
{
  if(!mqttClient.connected()) {
    mqttConnect();
  }
  motionDetection();

  char result[4];

  dtostrf(getOut, 4, 0, result);
  mqttClient.publish(getOutTopic, result);

  dtostrf(getIn, 4, 0, result);
  mqttClient.publish(getInTopic, result);

  mqttClient.loop();

  delay(500);
  // /*Function WARNING*/ 

  // float humid = humidity(dht);
  // float temp = temperature(dht, type);
  // delay(500);

  // char buffer [50];

  // //Publish humidity
  // snprintf(buffer, 50, "%.2lf", humid);
  // mqttClient.publish("IOT_SMARTDOOR/humid", buffer);
  // //Publish temperature
  // snprintf(buffer, 50, "%.2lf", temp);
  // mqttClient.publish("IOT_SMARTDOOR/temp", buffer);
  // //Publish type of temperature
  // snprintf(buffer, 50, "%s", type ? "true":"false");
  // mqttClient.publish("IOT_SMARTDOOR/type_temp", buffer);
  
  //  // Object detection - open/close door
  /*Function CONTROL*/
}
