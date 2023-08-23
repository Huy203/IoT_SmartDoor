#include "connWifi.h"
#include "lcd.h"
// #include "currentTime.h"

using namespace std;

// Init Firebase
int postId = 0;
FirebaseData firebaseData;
FirebaseJson jsonData;

// Init type of Temperature
TypeTemp type = TypeTemp::Fahrenheit;

// Init button alertTemp
int button = 25;
int buttonPressTime = 0;

// Init check alertTemp
bool alertTemp = false;

// void pushTimeToFirebase(struct tm *timeinfo)
// {
//   // Format the time and date strings
//   char timeStr[30];
//   strftime(timeStr, sizeof(timeStr), "%H:%M:%S %d/%m/%Y   %Z", timeinfo);

//   // Add the formatted strings to JSON
//   jsonData.add("alertTemp", timeStr);
//   if (Firebase.pushJSON(firebaseData, "/history/", jsonData))
//     Serial.println("Time data sent to Firebase successfully");
//   else
//   {
//     Serial.println("Failed to send time data to Firebase");
//     Serial.println(firebaseData.errorReason());
//   }
// }

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  // Push time and date data to Firebase
  // pushTimeToFirebase(&timeinfo);
}

void setup()
{
  Serial.begin(115200);

  // connect to Wifi Wokwi-Guest
  connectWifi();

  // connect to DHT22
  dht.begin();

  // set up for firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  if (Firebase.ready())
    Serial.println("Connected to Firebase");
  else
    Serial.println("Not connected to Firebase");

  // set up button alertTemp
  pinMode(button, INPUT);

  // Init
  lcd.init();
  lcd.backlight();

  // Print something
  lcd.setCursor(3, 0);
  lcd.print("LCD Start");

  // set up init parameters
  Firebase.setBool(firebaseData, "/door/tempAlert", alertTemp);
  Firebase.setBool(firebaseData, "/env/type", type);
  Firebase.setFloat(firebaseData, "/env/temperature", 0);
  Firebase.setFloat(firebaseData, "/env/humidity", 0);

  // set up time
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

void loop()
{
  jsonData.clear();
  bool buttonState = digitalRead(button);
  // set alert when button is pressed
  if (buttonState == HIGH)
  {
    if (millis() - buttonPressTime > 5000 || alertTemp)
    {
      LCDalert();
      Firebase.setBool(firebaseData, "/door/tempAlert", true);
      Serial.println("alert temp done");
    }
  }
  else
  {
    if (Firebase.getBool(firebaseData, "/door/tempAlert"))
    {
      alertTemp = firebaseData.boolData();
      if (!alertTemp)
      {
        // get data from firebase
        if (Firebase.getBool(firebaseData, "/env/type"))
        {
          TypeTemp newType = (TypeTemp)firebaseData.boolData();
          if (newType != type)
          {
            // set type of temperature
            type = newType;
            // setLCD(temp, humid, type);
            Serial.println("set new type of temperature done");
          }
        }
        float humid = humidity();
        float temp = temperature(type);
        if (Firebase.getFloat(firebaseData, "/env/temperature"))
        {
          float oldTemp = firebaseData.floatData();
          if (oldTemp != temp)
          {
            // push Temperature
            Firebase.setFloat(firebaseData, "/env/temperature", temp);
            if (setLCD(temp, humid, type) != 0)
            {
              Firebase.setBool(firebaseData, "/door/tempAlert", true);
            }
          }
        }
        if (Firebase.getFloat(firebaseData, "/env/humidity"))
        {
          float oldHumid = firebaseData.floatData();
          if (oldHumid != humid)
          {
            // push Humidity
            Firebase.setFloat(firebaseData, "/env/humidity", humid);
            setLCD(temp, humid, type);
            Serial.println("set new humidity done");
          }
        }
      }
    }
    printLocalTime();
    buttonPressTime = millis();
  }
}
