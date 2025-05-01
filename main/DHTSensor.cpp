#include "DHTSensor.hpp"


DHTSensor dht11(4, DHT11);


DHTSensor::DHTSensor(int dhtPin, int dhtType)
    : pin(dhtPin), type(dhtType), humidity(0.0), temperature(0.0), dht(dhtPin, dhtType) {}

void DHTSensor::begin() {
    dht.begin();
}

bool DHTSensor::readValues() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();


    if (isnan(h) || isnan(t)) {
        return false;
    }

    humidity = h;
    temperature = t;
    return true;
}

float DHTSensor::getHumidity() const {
    return humidity;
}

float DHTSensor::getTemperature() const {
    return temperature;
}

void DHTSensor::clearValues() {
    humidity = 0.0;
    temperature = 0.0;
}
