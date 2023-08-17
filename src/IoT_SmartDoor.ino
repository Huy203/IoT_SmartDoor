#include "connWifi.h"
#include "humidTemp.h"
#include "lcd.h"

using namespace std;

// Init Firebase
int postId = 0;
FirebaseData firebaseData;
FirebaseJson json;

// Init type of Temperature
TypeTemp type = TypeTemp::Cescius;

// Init button alertTemp
int button = 25;
int buttonPressTime = 0;

// Init check alertTemp
bool alertTemp = false;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  connectWifi(); // connect to Wifi Wokwi-Guest

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
  Firebase.setBool(firebaseData, "/door/alertTemp", alertTemp);
  Firebase.setBool(firebaseData, "/env/type", type);
  Firebase.setFloat(firebaseData, "/env/temperature", 0);
  Firebase.setFloat(firebaseData, "/env/humidity", 0);
}

void loop()
{
  delay(10); // this speeds up the simulation
  bool buttonState = digitalRead(button);
  Serial.println(buttonState);
  // set leds when button is pressed
  if (buttonState == HIGH)
  {
    Serial.println("HIGH");
    if (millis() - buttonPressTime > 5000 || alertTemp)
    {
      LCDalert();
      Firebase.setBool(firebaseData, "/door/alertTemp", true);
      Serial.println("alertTemp done");
    }
  }
  else
  {
    if (Firebase.getBool(firebaseData, "/door/alertTemp"))
    {
      alertTemp = firebaseData.boolData();
      if (!alertTemp)
      {
        float humid = humidity();
        float temp = temperature(type);

        // get data from firebase
        if (Firebase.getFloat(firebaseData, "/env/temperature"))
        {
          float oldTemp = firebaseData.floatData();
          if (oldTemp != temp)
          {
            // push Temperature
            Firebase.setFloat(firebaseData, "/env/temperature", temp);
            setLCD(temp, humid, type);
            Serial.println("set new temperature done");
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
        if (Firebase.getBool(firebaseData, "/env/type"))
        {
          TypeTemp newType = (TypeTemp)firebaseData.boolData();
          if (newType != type)
          {
            // set type of temperature
            type = newType;
            setLCD(temp, humid, type);
            Serial.println("set new type of temperature done");
          }
        }
        postId = postId + 1;
      }
    }
    buttonPressTime = millis();
  }
}