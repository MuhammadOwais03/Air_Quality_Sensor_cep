#ifndef DHT_SENSOR_HPP
#define DHT_SENSOR_HPP

#include <DHT.h>

class DHTSensor {
private:
    int pin;
    int type;
    float humidity;
    float temperature;
    DHT dht;

public:
    DHTSensor(int dhtPin, int dhtType);

    void begin();
    bool readValues();        // Returns true if read successful
    float getHumidity() const;
    float getTemperature() const;
    void clearValues();
};

#endif // DHT_SENSOR_HPP
