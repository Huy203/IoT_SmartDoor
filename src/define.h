#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <Arduino.h>
#include <FirebaseESP32.h>
#include <ctime>
#include <iostream>

using namespace std;

enum TypeTemp
{
    Cescius = false,
    Fahrenheit = true,
};

// FireBase
#define FIREBASE_HOST "https://testfirebase-165c4-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "AIzaSyC3HBYPCoFbrjZUk7O-XpU6YNw_vVnC_Xc"

// LCD
#define I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_LINES 4

// Time
#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     0
#define UTC_OFFSET_DST 0