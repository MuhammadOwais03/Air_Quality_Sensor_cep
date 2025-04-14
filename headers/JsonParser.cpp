#include "JsonParser.hpp"
#include <Arduino.h>

JsonParser::JsonParser(const String& jsonString) : jsonString(jsonString) {
    deserializeJson(doc, jsonString);
}

void JsonParser::getJsonValues(const char* key1, const char* key2, const char* key3) {
    if (key1 && doc.containsKey(key1)) ssid = doc[key1].as<String>();
    if (key2 && doc.containsKey(key2)) password = doc[key2].as<String>();
}
