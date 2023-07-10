#include "DHT.h"
#include <string.h>

void humidity(DHT dht)
{
    float humid = dht.readHumidity();
    Serial.print("Humidity: ");
    Serial.print(humid);
    Serial.println("%");
}
void temperature(DHT dht, bool type)
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
}