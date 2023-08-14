#include <PubSubClient.h>
#include <WiFi.h>
#include <Arduino.h>
#include <string>
#include "DHT.h"

using namespace std;

const int dhtPin = 22;
const int dhtType = DHT22; // Type of dht

enum TypeTemp
{
    Cescius = false,
    Fahrenheit = true,
};

static int numberPeopleInOneDay = 0;
static int oldNumberPeopleInOneDay = 0;

const int outsidePirPin = 2; // Pin of 2 PIR sensors
const int insidePirPin = 4;

DHT dht(dhtPin, dhtType);

//***Set server***
const char *mqttServer = "broker.hivemq.com";
int port = 1883;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

TypeTemp type = Cescius;

// function to connect Wifi
void connectWifi()
{
  WiFi.begin("Wokwi-GUEST", "");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
  }

  Serial.println("");

  Serial.println("WiFi connected");

  Serial.print("IP address: ");

  Serial.println(WiFi.localIP());
}

// function to connect MQTT
void mqttConnect()
{
  while (!mqttClient.connected())
  {
    Serial.println("Attemping MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str()))
    {
      Serial.println("connected");

      //***Subscribe all topic you need***
      //mqttClient.subscribe("IOT_SMARTDOOR/state_temp");
    }
    else
    {
      Serial.println("Try again in 2 seconds");
      delay(2000);
    }
  }
}

// MQTT Receiver
void callback(char *topic, byte *message, unsigned int length)
{
  Serial.println(topic);
  String strMsg;
  for (int i = 0; i < length; i++)
  {
    strMsg += (char)message[i];
  }
  Serial.println(strMsg);
  //***Code here to process the received package***

  // function Warning: receiving msg to check dangerous
  if (strMsg.compareTo("DANGEROUS"))
  {
    // Open the door
    // mqttClient.publist
  }
}

int outsideVal = 0; // read value of 2 PIR sensors
int insideVal = 0;

int outsidePirState = LOW; // state of 2 PIR sensors
int insidePirState = LOW;
int doorState = false; // state of door - false -> closed | true -> opened

void motionDetection()
{
  outsideVal = digitalRead(outsidePirPin);
  insideVal = digitalRead(insidePirPin);

  if (outsideVal == HIGH)
  {
    if (outsidePirState == LOW)
    {
      Serial.println("People from outside detected - Door opened");
      doorState = true;
      outsidePirState = HIGH;
      setNumberPeople();
    }
  }
  else
  {
    if (outsidePirState)
    {
      Serial.println("No people from outside");
      outsidePirState = LOW;
    }
  }

  if (insideVal == HIGH)
  {
    if (insidePirState == LOW)
    {
      Serial.println("People from inside detected - Door opened");
      doorState = true;
      insidePirState = HIGH;
      setNumberPeople();
    }
  }
  else
  {
    if (insidePirState)
    {
      Serial.println("No people from inside");
      insidePirState = LOW;
    }
  }

  if (outsidePirState == LOW && insidePirState == LOW && doorState)
  {
    Serial.println("No people detected - Door closed");
    doorState = false;
  }
}

//function to get the number of people who are checked by PIR
void setNumberPeople(){
  numberPeopleInOneDay++ ;
}
// function to get humidity
float humidity(DHT dht)
{
  float humid = dht.readHumidity();
  Serial.print("Humidity: ");
  Serial.print(humid);
  Serial.println("%");
  return humid;
}

// function to get temperature
float temperature(DHT dht, bool type)
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

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  connectWifi(); // connect to Wifi Wokwi-Guest
  pinMode(outsidePirPin, INPUT);
  pinMode(insidePirPin, INPUT);
  dht.begin();
  mqttClient.setServer(mqttServer, port);
  mqttClient.setCallback(callback);
  mqttClient.setKeepAlive(90);
}

void loop()
{
  delay(10); // this speeds up the simulation
  if (!mqttClient.connected())
  {
    mqttConnect();
  }
  mqttClient.loop();

  /*Function WARNING*/

  float humid = humidity(dht);
  float temp = temperature(dht, type);
  delay(500);

  char buffer[50];

  // Publish humidity
  snprintf(buffer, 50, "%.2lf", humid);
  mqttClient.publish("IOT_SMARTDOOR/humid", buffer);
  // Publish temperature
  snprintf(buffer, 50, "%.2lf", temp);
  mqttClient.publish("IOT_SMARTDOOR/temp", buffer);
  // Publish type of temperature
  snprintf(buffer, 50, "%s", type ? "true" : "false");
  mqttClient.publish("IOT_SMARTDOOR/type_temp", buffer);

  // Object detection - open/close door
  motionDetection();

  if(numberPeopleInOneDay == 0)
  {
    mqttClient.publish("IOT_SMARTDOOR/number_people", "0");
  }
  if(oldNumberPeopleInOneDay < numberPeopleInOneDay)
  {
    oldNumberPeopleInOneDay = numberPeopleInOneDay;
    snprintf(buffer, 50, "%d", numberPeopleInOneDay);
    mqttClient.publish("IOT_SMARTDOOR/number_people", buffer);
  }
}