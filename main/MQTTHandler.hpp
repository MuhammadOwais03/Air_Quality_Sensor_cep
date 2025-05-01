#ifndef MQTTHANDLER_HPP
#define MQTTHANDLER_HPP

#include <PubSubClient.h>
#include <WiFi.h>
#include <functional>

class MQTTHandler {
public:
    MQTTHandler(const char *brokerAddress, const char *subTopic, int ledPin);

    void setupMQTT();
    void loop();
    void setOTACallback(std::function<void()> callback);
    void setControlCallback(std::function<void(const String&)> callback);

private:
    const char *broker;
    const char *topic;
    int ledPin;
    WiFiClient wifiClient;
    PubSubClient client;
    long lastMsgTime;
    std::function<void()> otaCallback;
    std::function<void(const String&)> controlCallback;

    void mqttCallback(char *topic, byte *message, unsigned int length);
    void handleMessage(String topicReceived, String message);
    void controlLED(bool state);
    void reconnectMQTT();
    String getTemperatureHumidtyJSON();
};

#endif
