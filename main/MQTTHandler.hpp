#ifndef MQTT_HANDLER_HPP
#define MQTT_HANDLER_HPP

#include <WiFi.h>
#include <PubSubClient.h>

class MQTTHandler {
public:
    MQTTHandler(const char* brokerAddress, const char* subTopic, int ledPin = 2);

    void setupMQTT();   // Setup MQTT client and connect to broker
    void loop();        // Keep MQTT connection alive and handle messages
    void mqttCallback(char* topic, byte* message, unsigned int length); // MQTT message handler
    String getTemperatureHumidtyJSON();

private:
    void reconnectMQTT();                       // Handle MQTT reconnection
    void handleMessage(String topic, String message);
    void controlLED(bool state);

    const char* broker;
    const char* topic;
    int ledPin;
    long lastMsgTime;
    

    WiFiClient wifiClient;
    PubSubClient client;
};

extern MQTTHandler mqtt;

#endif // MQTT_HANDLER_HPP
