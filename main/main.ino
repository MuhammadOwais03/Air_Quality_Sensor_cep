



#include <Arduino.h>
#include "WifiHandler.hpp"
#include "MQTTHandler.hpp"
#include "DHTSensor.hpp"

// // Create WiFi handler instance


// Function prototypes
// void WiFiTask(void* parameter);
void SensorTask(void* parameter);
const char* mqttBroker = "broker.emqx.io";
const char* mqttTopic = "esp32/iot_temp";

MQTTHandler mqtt(mqttBroker, mqttTopic, 2);
WiFiHandler wifi("ESP32_AP", "password123", &mqtt);




void setup() {
  Serial.begin(115200);
  delay(1000);
  wifi.create(); 
  while (!wifi.isWifiConnected) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");

  mqtt.setupMQTT();


  // Create Sensor task on Core 1
  xTaskCreatePinnedToCore(
    SensorTask,
    "Sensor Task",
    10000,
    NULL,
    1,
    NULL,
    1                  // Core 1
  );
}

void loop() {
  // Not used, everything runs in tasks
  mqtt.loop();
}


// void WiFiTask(void* parameter) {
//   wifi.connect(); // Initial connection

//   while (true) {
//     wifi.handleReconnect(); // Keep checking connection
//     vTaskDelay(5000 / portTICK_PERIOD_MS); // Check every 5s
//   }
// }


void SensorTask(void* parameter) {
  dht11.begin();

  while (true) {
    dht11.readValues();
   
   

    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  }
}





