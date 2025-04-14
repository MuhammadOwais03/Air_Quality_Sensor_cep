#ifndef JSONPARSER_HPP
#define JSONPARSER_HPP

#include <ArduinoJson.h>

class JsonParser {
public:
    JsonParser(const String& jsonString);
    void getJsonValues(const char* key1, const char* key2 = nullptr, const char* key3 = nullptr);

    String ssid;
    String password;

private:
    String jsonString;
    StaticJsonDocument<200> doc;
};

#endif
