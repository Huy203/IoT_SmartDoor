# 1 "/var/folders/1q/0b6t78h13_gbphk9yrqbqlzc0000gn/T/tmp6dimcaet"
#include <Arduino.h>
# 1 "/Users/henry/Documents/Năm2_Kì3/IOT/IoT_SmartDoor2/src/IoT_SmartDoor.ino"
#include "connWifi.h"
#include "humidTemp.h"
#include "lcd.h"


using namespace std;


int postId = 0;
FirebaseData firebaseData;
FirebaseJson jsonData;


TypeTemp type = TypeTemp::Cescius;


int button = 25;
int buttonPressTime = 0;


bool alertTemp = false;
void pushTimeToFirebase(struct tm *timeinfo);
void printLocalTime();
void setup();
void loop();
#line 23 "/Users/henry/Documents/Năm2_Kì3/IOT/IoT_SmartDoor2/src/IoT_SmartDoor.ino"
void pushTimeToFirebase(struct tm *timeinfo)
{

  char timeStr[30];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S %d/%m/%Y   %Z", timeinfo);


  jsonData.add("alertTemp", timeStr);
  if (Firebase.pushJSON(firebaseData, "/history/", jsonData))
    Serial.println("Time data sent to Firebase successfully");
  else
  {
    Serial.println("Failed to send time data to Firebase");
    Serial.println(firebaseData.errorReason());
  }
}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }

  pushTimeToFirebase(&timeinfo);
}

void setup()
{
  Serial.begin(115200);


  connectWifi();


  dht.begin();


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  if (Firebase.ready())
    Serial.println("Connected to Firebase");
  else
    Serial.println("Not connected to Firebase");


  pinMode(button, INPUT);


  lcd.init();
  lcd.backlight();


  lcd.setCursor(3, 0);
  lcd.print("LCD Start");


  Firebase.setBool(firebaseData, "/door/tempAlert", alertTemp);
  Firebase.setBool(firebaseData, "/env/type", type);
  Firebase.setFloat(firebaseData, "/env/temperature", 0);
  Firebase.setFloat(firebaseData, "/env/humidity", 0);


  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

void loop()
{
  jsonData.clear();
  bool buttonState = digitalRead(button);

  if (buttonState == HIGH)
  {
    if (millis() - buttonPressTime > 5000 || alertTemp)
    {
      LCDalert();
      Firebase.setBool(firebaseData, "/door/tempAlert", true);
      Serial.println("alertTemp done");
    }
  }
  else
  {
    if (Firebase.getBool(firebaseData, "/door/tempAlert"))
    {
      alertTemp = firebaseData.boolData();
      if (!alertTemp)
      {
        float humid = humidity();
        float temp = temperature(type);


        if (Firebase.getFloat(firebaseData, "/env/temperature"))
        {
          float oldTemp = firebaseData.floatData();
          if (oldTemp != temp)
          {

            Firebase.setFloat(firebaseData, "/env/temperature", temp);
            if (setLCD(temp, humid, type) != 0)
            {
              Firebase.setBool(firebaseData, "/door/tempAlert", true);
            }
            Serial.println("set new temperature done");
          }
        }
        if (Firebase.getFloat(firebaseData, "/env/humidity"))
        {
          float oldHumid = firebaseData.floatData();
          if (oldHumid != humid)
          {

            Firebase.setFloat(firebaseData, "/env/humidity", humid);
            setLCD(temp, humid, type);
            Serial.println("set new humidity done");
          }
        }
        if (Firebase.getBool(firebaseData, "/env/type"))
        {
          TypeTemp newType = (TypeTemp)firebaseData.boolData();
          if (newType != type)
          {

            type = newType;
            setLCD(temp, humid, type);
            Serial.println("set new type of temperature done");
          }
        }
      }
    }
    buttonPressTime = millis();
  }
  printLocalTime();
}