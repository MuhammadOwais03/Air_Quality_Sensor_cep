#include "MQTTHandler.hpp"
#include "DHTSensor.hpp"
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

MQTTHandler::MQTTHandler(const char *brokerAddress, const char *subTopic, int ledPinPin)
    : broker(brokerAddress), topic(subTopic), ledPin(ledPinPin), client(wifiClient), lastMsgTime(0) {}

void MQTTHandler::setupMQTT() {
    pinMode(ledPin, OUTPUT);
    client.setServer(broker, 1883);
    client.setCallback([this](char *topic, byte *message, unsigned int length) {
        this->mqttCallback(topic, message, length);
    });
}

void MQTTHandler::setOTACallback(std::function<void()> callback) {
    otaCallback = callback;
}

void MQTTHandler::mqttCallback(char *topic, byte *message, unsigned int length) {
    String messageTemp;
    for (unsigned int i = 0; i < length; i++) {
        messageTemp += (char)message[i];
    }
    handleMessage(String(topic), messageTemp);
}

// void MQTTHandler::handleMessage(String topicReceived, String message) {
//   Serial.println(message);
//   if (message == "New firmware uploaded") {
//             Serial.println(message);
//             if (otaCallback) {
//               Serial.println("ENTER");
//                 otaCallback();  // Trigger OTA update
//             }}
//     if (topicReceived == topic) {
//         if (message == "on") {
//             controlLED(true);
//         } else if (message == "off") {
//             controlLED(false);
//         } else if (message.message == "New firmware uploaded") {
//             Serial.println(message);
//             if (otaCallback) {
//               Serial.println("ENTER");
//                 otaCallback();  // Trigger OTA update
//             }
//         }
//     }
// }

void MQTTHandler::handleMessage(String topicReceived, String message) {
    Serial.println("Received: " + message);

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        return;
    }

    // Check if it's a firmware message
    if (doc.containsKey("message")) {
        String msg = doc["message"];
        if (msg == "New firmware uploaded") {
            Serial.println("Firmware update triggered");
            if (otaCallback) {
                Serial.println("ENTER");
                otaCallback();  // Trigger OTA update
            }
        }
        return; // No need to check further
    }

    // Check if it's sensor data
    if (doc.containsKey("temperature") && doc.containsKey("humidity")) {
        Serial.println(message);
        return;
    }

    // Check for LED control messages
    if (topicReceived == topic && doc.containsKey("message")) {
        String cmd = doc["message"];
        if (cmd == "on") {
            controlLED(true);
        } else if (cmd == "off") {
            controlLED(false);
        }
    }
}

void MQTTHandler::controlLED(bool state) {
    digitalWrite(ledPin, state ? HIGH : LOW);
}

void MQTTHandler::reconnectMQTT() {
    while (!client.connected()) {
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        if (client.connect(clientId.c_str())) {
            client.subscribe(topic);
        } else {
            delay(5000);
        }
    }
}

String MQTTHandler::getTemperatureHumidtyJSON() {
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "{\"temperature\": %.2f, \"humidity\": %.2f}", dht11.temperature, dht11.humidity);
    return String(buffer);
}

void MQTTHandler::loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();

    long now = millis();

    // Publish every 5 seconds
    if (now - lastMsgTime > 5000) {
        lastMsgTime = now;

        String payload = getTemperatureHumidtyJSON();
        client.publish(topic, payload.c_str());

        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin("http://192.168.100.42:5000/api/temp");
            http.addHeader("Content-Type", "application/json");
            http.POST(payload);
            http.end();
        }
    }
}
