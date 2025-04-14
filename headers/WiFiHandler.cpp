#include "WiFiHandler.hpp"
#include <Arduino.h>

WiFiProvider::WiFiProvider() {
    WiFi.mode(WIFI_STA);  // Station mode
    WiFi.disconnect(true);  // Disconnect any previous connection
    delay(100);
}

void WiFiProvider::connect(String& ssid_val, String& password_val) {
   
    ssid = ssid_val;
    password = password_val;

    connectToWiFi();
}

void WiFiProvider::connectToWiFi() {
    Serial.print("Connecting to WiFi SSID: ");
    Serial.println(ssid);

    WiFi.begin(ssid.c_str(), password.c_str());

    int maxAttempts = 20;
    while (WiFi.status() != WL_CONNECTED && maxAttempts-- > 0) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected successfully!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to WiFi.");
    }
}

bool WiFiProvider::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}
