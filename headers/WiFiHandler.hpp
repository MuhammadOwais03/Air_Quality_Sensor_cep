#ifndef WIFI_HANDLER_HPP
#define WIFI_HANDLER_HPP

#include <WiFi.h>
// #include "JsonParser.hpp"

class WiFiProvider
{ 
public:
WiFiProvider();  // Constructor

    // Attempts to connect using JSON string containing SSID and password
    void connect(String& ssid_val, String& password_val);

    // Returns true if connected to WiFi
    bool isConnected() const;

private:
    String ssid;
    String password;

    void connectToWiFi();
};

#endif // WIFI_HANDLER_HPP
