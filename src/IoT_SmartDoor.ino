// #include <PubSubClient.h>
#include <Arduino.h>
#include "DHT.h"
#include <FirebaseESP32.h>

#include "connWifi.h"
#include "define.h"
#include "humidTemp.h"
#include "lcd.h"

using namespace std;

// Init Firebase
int postId = 0;
FirebaseData firebaseData;
FirebaseJson json;

// Init type of Temperature
TypeTemp type = TypeTemp::Cescius;

// Init button alarm
int button = 25;
int buttonPressTime = 0;

// Init check alarm
String check = "";

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  connectWifi(); // connect to Wifi Wokwi-Guest

  dht.begin();

  // set up for firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  // set up button alarm
  pinMode(button, INPUT);

  // Init
  lcd.init();
  lcd.backlight();

  // Print something
  lcd.setCursor(3, 0);
  lcd.print("LCD Start");

  // set up type of temperature
  Firebase.setBool(firebaseData, "/env/type", type);
  Firebase.setString(firebaseData, "/door/alarm", check);
}

void loop()
{
  delay(10); // this speeds up the simulation
  bool buttonState = digitalRead(button);
  // set leds when button is pressed
  if (buttonState == HIGH)
  {
    Serial.println("Pressed");
    if (millis() - buttonPressTime > 5000 || check == "Alarm")
    {
      LCDalarm();
      Firebase.setString(firebaseData, "/door/alarm", "Alarm");
      Serial.println("Alarm done");
    }
  }
  else
  {
    if (Firebase.getString(firebaseData, "/door/alarm"))
    {
      check = firebaseData.stringData();
      if (check != "Alarm")
      {
        /*Function WARNING*/
        float humid = humidity();
        float temp = temperature(type);

        // get data from firebase
        if (Firebase.getFloat(firebaseData, "/env/temperature"))
        {
          float oldTemp = firebaseData.floatData();
          if (oldTemp != temp)
          { // push Temperature
            Firebase.setFloat(firebaseData, "/env/temperature", temp);
            setLCD(temp, humid, type);
            Serial.println("set new temperature done");
          }
        }
        if (Firebase.getFloat(firebaseData, "/env/humidity"))
        {
          float oldHumid = firebaseData.floatData();
          if (oldHumid != humid)
          { // push Humidity
            Firebase.setFloat(firebaseData, "/env/humidity", humid);
            setLCD(temp, humid, type);
            Serial.println("set new humidity done");
          }
        }
        if (Firebase.getBool(firebaseData, "/env/type"))
        {
          TypeTemp newType = (TypeTemp)firebaseData.boolData();
          if (newType != type)
          { // set type of temperature
            type = newType;
            setLCD(temp, humid, type);
            Serial.println("set new type of temperature done");
          }
        }

        // set data to database
        postId = postId + 1;
        // get data from firebase
        /*
          // get value type Int
          Firebase.getInt(firebaseData, "/Label");
          // assign value into variable
          variable = Firebase.intData();
        */
      }
    }
    buttonPressTime = millis();
  }
}