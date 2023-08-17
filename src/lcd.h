#include "define.h"

static LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

int postID = 0;

byte degreeSymbol[8] = {
    B00110,
    B01001,
    B00110,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000};

void LCDalert()
{
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("DANGEROUS!");
}

void LCDreset()
{
    postID = 0;
}

int setLCD(float temp, float humid, TypeTemp type)
{
    if (type && temp > 150)
    {
        postID++;
    }
    if (!type && temp > 60)
    {
        postID++;
    }
    if (postID == 0)
    {
        ostringstream stream;
        stream << fixed << setprecision(2) << temp;

        string strTemp;
        strTemp = "Temp: " + stream.str();

        lcd.clear();
        const char *buffer = strTemp.c_str();
        lcd.createChar(1, degreeSymbol);
        lcd.setCursor(0, 0);
        lcd.print(buffer);
        lcd.write(1);
        if (type)
            lcd.print("F");
        else
            lcd.print("C");

        stream.str("");
        stream << fixed << setprecision(2) << humid;
        string strHumid = "Humid: " + stream.str() + "%";
        buffer = strHumid.c_str();
        lcd.setCursor(0, 1);
        lcd.print(buffer);
    }
    else
        LCDalert();

    return postID;
}
