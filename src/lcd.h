#include "define.h"

static LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

int postID = 0;

void LCDalarm()
{
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("DANGEROUS!");
}

void LCDreset(){
    postID = 0;
}

void setLCD(float temp, float humid, TypeTemp type)
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
        if (type)
            strTemp = "Temp: " + stream.str() + "°F";
        // Serial.println("°F"); // °: Option/Alt +Shift + 8
        else
            // Serial.println("°C");
            strTemp = "Temp: " + stream.str() + "°C";

        lcd.clear();
        const char *buffer = strTemp.c_str();
        lcd.setCursor(0, 0);
        lcd.print(buffer);

        stream.str("");
        stream << fixed << setprecision(2) << humid;
        string strHumid = "Humid: " + stream.str() + "%";
        buffer = strHumid.c_str();
        lcd.setCursor(0, 1);
        lcd.print(buffer);
    }
    else
        LCDalarm();
}
