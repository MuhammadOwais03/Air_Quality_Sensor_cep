#ifndef DHT_SENSOR_HPP
#define DHT_SENSOR_HPP

#include <DHT.h>

class DHTSensor
{
private:
    int pin;
    int type;
    DHT dht;

public:
    float humidity;
    float temperature;
    DHTSensor(int dhtPin, int dhtType);

    void begin();
    bool readValues(); // Returns true if read successful
    float getHumidity() const;
    float getTemperature() const;
    void clearValues();
};

extern DHTSensor dht11;

#endif // DHT_SENSOR_HPP
