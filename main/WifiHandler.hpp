#ifndef WIFI_HANDLER_HPP
#define WIFI_HANDLER_HPP

#include <WiFiManager.h>
#include <WiFi.h>

// class WiFiHandler {
// public:
//     WiFiHandler(const char* apName = "ESP32_AP", const char* apPassword = "password123");

//     void connect();                              // Connect using WiFiManager
//     void handleReconnect(unsigned long timeout = 10000); // Reconnect if disconnected
//     void resetWiFi();                            // Clear saved credentials
//     void create();                               

//     WiFiManager wm;

// private:
//     const char* apName;
//     const char* apPassword;

//     static void WiFiTask(void* parameter); // ✅ Static FreeRTOS task function
// };

#include "MQTTHandler.hpp"  // Add this include

class WiFiHandler {
public:
   // WifiHandler.hpp
WiFiHandler(const char* apName, const char* apPassword, MQTTHandler* mqtt);


    void connect();
    void handleReconnect(unsigned long timeout = 10000);
    void resetWiFi();
    void create();

    WiFiManager wm;
    bool isWifiConnected = false;
private:
    static void WiFiTask(void* parameter);  // now static
    const char* apName;
    const char* apPassword;
    MQTTHandler* mqtt; 
};

#endif // WIFI_HANDLER_HPP
