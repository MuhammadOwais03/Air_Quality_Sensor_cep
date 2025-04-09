#include "JsonParser.hpp"
#include <ArduinoJson.h>
#include <Arduino.h>

JsonParser::JsonParser(const String& jsonString) : jsonString(jsonString) {
    // Deserialize the JSON string
    DeserializationError error = deserializeJson(doc, jsonString);

    // Check if there was an error in parsing the JSON
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
    }
}

// Getter function that takes 1, 2, or 3 JSON keys
void JsonParser::getJsonValues(const char* key1, const char* key2, const char* key3) {
    // Check if the key1 exists and print its value
    if (doc.containsKey(key1)) {
        const char* value1 = doc[key1];
        Serial.print(key1);
        Serial.print(": ");
        Serial.println(value1);
    }

    // Check if key2 exists and print its value
    if (key2 && doc.containsKey(key2)) {
        const char* value2 = doc[key2];
        Serial.print(key2);
        Serial.print(": ");
        Serial.println(value2);
    }

    // Check if key3 exists and print its value
    if (key3 && doc.containsKey(key3)) {
        const char* value3 = doc[key3];
        Serial.print(key3);
        Serial.print(": ");
        Serial.println(value3);
    }
}
