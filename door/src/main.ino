#include <Arduino.h>
#include <FirebaseESP32.h>
#include <WiFi.h>
#include <ESP32Servo.h>

#define FIREBASE_HOST "https://testfirebase-165c4-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "AIzaSyC3HBYPCoFbrjZUk7O-XpU6YNw_vVnC_Xc"

FirebaseData firebaseData;
FirebaseJson json;

// set device pin
const int outsidePirPin = 25; 
const int insidePirPin = 5;

const int outsideServoPin = 32;
const int insideServoPin = 19;

// set device
Servo outsideDoor;
Servo insideDoor;

// set global variable
int outsideVal = 0; // read value of 2 PIR sensors
int insideVal = 0;

int outsidePirState = LOW; // state of 2 PIR sensors
int insidePirState = LOW;
int doorState = false; // state of door - false -> closed | true -> opened

int getIn = 0; // number of people got into and got out
int getOut = 0;

bool tempAlert = false; // for temperature alert
bool tempAlertState = false; // state of temperature alert - false -> no alert | true -> alert

bool onSecurity = false; // mode security of door

bool intruderAlert = false; // for intruder alert

int previousMillis = 0; // for push in cyclically

String control = "auto"; // door mode

// connect to Wokwi Wifi
void connectWifi(){
  WiFi.begin("Wokwi-GUEST", "");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(250);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// open door instruction
void openDoor(){
  doorState = true;
  insideDoor.write(0);
  outsideDoor.write(0);
}

// close door instruction
void closeDoor(){
  doorState = false;
  insideDoor.write(90);
  outsideDoor.write(90);
}

// read sensor and handle value
void handleSensor(){
  // read sensor value
  outsideVal = digitalRead(outsidePirPin);
  insideVal = digitalRead(insidePirPin);

  if(outsideVal == HIGH){
    if(outsidePirState == LOW){
      Serial.println("People from outside detected");
      outsidePirState = HIGH;
      if(!onSecurity){
        // if not on security -> open door
        openDoor();
        if(insideVal == HIGH) // if there's people from inside -> people getting out of the building
          getOut++;
      }
      else {
        // if on security -> set alert of intruder
        if(!intruderAlert){
          intruderAlert = true;
          Serial.println("Intruder Alert!!!");
        }
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
        // if not on security -> open door
        openDoor();
        if(outsideVal == HIGH) // if there's people from outside -> people getting into the building
          getIn++;
      }
      else {
        // if on security -> set alert of intruder
        if(!intruderAlert){
          intruderAlert = true;
          Serial.println("Intruder Alert!!!");
        }
      }
    }
  }
  else {
    if(insidePirState){
      Serial.println("No people from inside");
      insidePirState = LOW;
    }
  }

  if(outsidePirState == LOW && insidePirState == LOW){
    if(doorState){
      Serial.println("No people detected");
      closeDoor();
    }
    if(onSecurity && intruderAlert){
      Serial.println("No intruder detected");
      intruderAlert = false;
    }
  }
}

// get signal from web on handle it
void handleSignal(){
  // read signal of temperature alert
  if(Firebase.getBool(firebaseData, "door/tempAlert")){
    tempAlert = firebaseData.boolData();
  }
  
  // if on temperature alert, open door
  if(tempAlert){
    if(!tempAlertState){
      tempAlertState = true;
      openDoor();
    }
  }
  else {
    // else, read other signal
    if(tempAlertState)
      tempAlertState = false;
    
    // read signal of security mode
    if(Firebase.getBool(firebaseData, "door/security")){
      onSecurity = firebaseData.boolData();
    }

    if(onSecurity){
      // detect possible intruder if on security mode
      if(Firebase.getBool(firebaseData, "door/intruderAlert")){
        intruderAlert = firebaseData.boolData();
      }
    }

    // get signal of orther door mode
    if(Firebase.getString(firebaseData, "door/control")){
      control = firebaseData.stringData();
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectWifi();

  // set up device
  insideDoor.attach(insideServoPin);
  outsideDoor.attach(outsideServoPin);

  // connect to firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  if (Firebase.ready())
    Serial.println("Connected to Firebase");
  else
    Serial.println("Not connected to Firebase");

  // read assigned value
  if(Firebase.getInt(firebaseData, "/door/monitor/getIn")){
    getIn = firebaseData.intData();
    Firebase.setInt(firebaseData, "/door/monitor/getIn", getIn);
  }

  if(Firebase.getInt(firebaseData, "/door/monitor/getOut")){
    getOut = firebaseData.intData();
    Firebase.setInt(firebaseData, "/door/monitor/getOut", getOut);
  }

  if(Firebase.getBool(firebaseData, "/door/security")){
    onSecurity = firebaseData.boolData();
  }

  if(Firebase.getString(firebaseData, "door/control")){
    control = firebaseData.stringData();
  }
}

void loop() {
  handleSignal();
  
  if(!tempAlertState){
    if(control == "auto"){
      handleSensor();

      if(onSecurity){
        if(Firebase.getBool(firebaseData, "door/intruderAlert")){
          int old_intruderAlert = firebaseData.boolData();
          if(old_intruderAlert != intruderAlert){ 
            Firebase.setBool(firebaseData, "door/intruderAlert", intruderAlert);
            Serial.println("Set new intruder alert");
          }
        }
      }
    }
    else if(control == "open"){
      openDoor();
    }
    else if(control == "close"){
      closeDoor();
    }
  }
  else
    Serial.println("DANGEROUS!!!");

  if(!onSecurity && intruderAlert){
    intruderAlert = false;
    Firebase.setBool(firebaseData, "door/intruderAlert", intruderAlert);
    Serial.println("Set new intruder alert");
  }

  if(millis() - previousMillis > 1 * 1000){
    if(Firebase.getInt(firebaseData, "/door/monitor/getIn")){
      int old_getInt = firebaseData.intData();
      if(old_getInt != getIn){
        Firebase.setInt(firebaseData, "/door/monitor/getIn", getIn);
        Serial.println("Set new get-in count");
      }
    }

    if(Firebase.getInt(firebaseData, "/door/monitor/getOut")){
      int old_getOut = firebaseData.intData();
      if(old_getOut != getOut){
        Firebase.setInt(firebaseData, "/door/monitor/getOut", getOut);
        Serial.println("Set new get-out count");
      }
    }
    previousMillis = millis();
  } 
  delay(500);
}
