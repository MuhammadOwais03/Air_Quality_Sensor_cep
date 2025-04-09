#include "JsonParser.hpp"

String jsonString = "{\"ssid\":\"SHAH Storm Fiber 0321-7162451\", \"password\":\"google700\", \"device\":\"ESP32\"}";

JsonParser parser(jsonString);  // Create an object of JsonParser

void setup() {
  Serial.begin(115200);
  Serial.println("Starting JSON parsing!");

  // Get values for ssid and password (2 keys)
  parser.getJsonValues("ssid", "password");

  // Get values for ssid, password, and device (3 keys)
  parser.getJsonValues("ssid", "password", "device");
}

void loop() {
  // No need to repeat code in loop for this example
}
