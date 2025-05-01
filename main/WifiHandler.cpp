#include "WifiHandler.hpp"




WiFiHandler::WiFiHandler(const char* apName, const char* apPassword, MQTTHandler* mqtt)
    : apName(apName), apPassword(apPassword), mqtt(mqtt) {}

void WiFiHandler::connect() {
    Serial.println("Initializing WiFiManager...");
    bool res = wm.autoConnect(apName, apPassword);
    if (!res) {
        Serial.println("Failed to connect and hit timeout.");
        ESP.restart();
    } else {
        Serial.println("Connected to WiFi!");
        Serial.println(WiFi.localIP());
        isWifiConnected = true;
        
    }
}

void WiFiHandler::handleReconnect(unsigned long timeout) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected! Attempting reconnect...");
        WiFi.begin(); // Uses saved credentials

        unsigned long startTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeout) {
            delay(500);
            Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nReconnected successfully!");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("\nReconnection failed.");
            resetWiFi();
        }
    }
}

void WiFiHandler::resetWiFi() {
    wm.resetSettings();
    Serial.println("WiFi credentials erased. Rebooting...");
    delay(1000);
    ESP.restart();
}

void WiFiHandler::create() {
    Serial.println("WiFi-MQTT task created");

    // Run both WiFi and MQTT on Core 0
    xTaskCreatePinnedToCore(
        WiFiTask,
        "WiFi & MQTT Task",
        10000,
        this,
        1,
        NULL,
        0  // ✅ Core 0
    );
}
void WiFiHandler::WiFiTask(void* parameter) {
    WiFiHandler* handler = static_cast<WiFiHandler*>(parameter);

    handler->connect();
    
    while (1) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
}

