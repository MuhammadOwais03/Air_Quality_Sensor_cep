#include "MQTTHandler.hpp"
#include "DHTSensor.hpp"
#include <Arduino.h>

MQTTHandler::MQTTHandler(const char *brokerAddress, const char *subTopic, int ledPinPin)
    : broker(brokerAddress), topic(subTopic), ledPin(ledPinPin), client(wifiClient), lastMsgTime(0)
{
}

void MQTTHandler::setupMQTT()
{
    pinMode(ledPin, OUTPUT);
    client.setServer(broker, 1883);

    // Use a lambda wrapper to call the member callback
    client.setCallback([this](char *topic, byte *message, unsigned int length)
                       { this->mqttCallback(topic, message, length); });
}

void MQTTHandler::mqttCallback(char *topic, byte *message, unsigned int length)
{
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");

    String messageTemp;
    for (unsigned int i = 0; i < length; i++)
    {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    handleMessage(String(topic), messageTemp);
}

void MQTTHandler::handleMessage(String topicReceived, String message)
{
    if (topicReceived == topic)
    {
        Serial.print("Changing output to ");
        if (message == "on")
        {
            Serial.println("on");
            controlLED(true);
        }
        else if (message == "off")
        {
            Serial.println("off");
            controlLED(false);
        }
    }
}

void MQTTHandler::controlLED(bool state)
{
    digitalWrite(ledPin, state ? HIGH : LOW);
}

void MQTTHandler::reconnectMQTT()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);

        if (client.connect(clientId.c_str()))
        {
            Serial.println("connected");
            client.subscribe(topic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

String MQTTHandler::getTemperatureHumidtyJSON()
{

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "{\"temperature\": %.2f, \"humidity\": %.2f}", dht11.temperature, dht11.humidity);

    return String(buffer);
}

void MQTTHandler::loop()
{
    if (!client.connected())
    {
        reconnectMQTT();
    }
    client.loop();

    long now = millis();
    if (now - lastMsgTime > 5000)
    {
        lastMsgTime = now;

        String tempPayload = getTemperatureHumidtyJSON();
        Serial.print("DHT JSON: ");
        Serial.println(tempPayload);

        client.publish(topic, tempPayload.c_str());
    }
}
