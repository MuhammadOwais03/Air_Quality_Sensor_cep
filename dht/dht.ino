#include "DHTSensor.hpp"

#define DHTPIN 4
#define DHTTYPE DHT11

DHTSensor mySensor(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("DHT11 Class Test");
    mySensor.begin();
}

void loop() {
    delay(2000);

    if (mySensor.readValues()) {
        Serial.print("Humidity: ");
        Serial.print(mySensor.getHumidity());
        Serial.print("%  Temperature: ");
        Serial.print(mySensor.getTemperature());
        Serial.println("°C");
    } else {
        Serial.println("❌ Failed to read from DHT sensor!");
    }
}
