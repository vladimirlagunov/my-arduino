#include <DHT.h>

/*
  DHT11 QC:
  S (left) = data
  + (middle) = 5V
  - (right) = GND
 */

const uint8_t dht_pin = 2;
DHT dht(dht_pin, DHT11);

void setup()
{
    Serial.begin(9600);
    dht.begin();
}

char buffer[256];
int temp = 0;
int hum = 0;

void loop()
{
    delay(2000);
    temp = dht.readTemperature() * 100.0;
    hum = dht.readHumidity() * 100.0;
    sprintf(buffer, "Temperature: %d.%02d°C  Humidity: %d.%02d%%\r\n", temp / 100, temp % 100, hum / 100, hum % 100);
    Serial.print(buffer);
    Serial.flush();
}
