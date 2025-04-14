#include <Arduino.h>
#include "BLEServiceHandler.hpp"
// #include "../wifi/WiFiHandler.cpp"
// #include "../JsonParser/JsonParser.cpp"

BLEServiceHandler bleService("MyESP32");

void setup() {
  Serial.begin(115200);
  bleService.init();
}

void loop() {
  delay(2000);
}
