#ifndef JSONPARSER_HPP
#define JSONPARSER_HPP

#include <ArduinoJson.h>

class JsonParser {
public:
    // Constructor: Takes the JSON string as an argument
    JsonParser(const String& jsonString);

    // Getter function to get values from JSON
    void getJsonValues(const char* key1, const char* key2 = nullptr, const char* key3 = nullptr);

private:
    String jsonString;   // The original JSON string
    StaticJsonDocument<200> doc;  // JSON document for parsing
};

#endif
