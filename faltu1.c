



#include <Arduino.h>
#include "WifiHandler.hpp"
#include "MQTTHandler.hpp"
#include "DHTSensor.hpp"

// // Create WiFi handler instance

const String baseURL = "http://192.168.100.42:5000/api/get-firmware?file=";

bool fetchAndUpdate(const String& filename) {
  String fullURL = baseURL + filename;
  HTTPClient http;
  http.begin(fullURL);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    WiFiClient *stream = http.getStreamPtr();

    if (Update.begin(contentLength)) {
      size_t written = Update.writeStream(*stream);
      if (written == contentLength && Update.end() && Update.isFinished()) {
        Serial.println(filename + " update complete.");
        return true;
      }
    }
  }

  Serial.println("Failed to update: " + filename);
  return false;
}

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

  if (Wifi.status() != WL_CONNECTED) {
    bool success = fetchAndUpdate("main.ino.partitions.bin") &&
                 fetchAndUpdate("main.ino.merged.bin") &&
                 fetchAndUpdate("main.ino.bin");

  if (success) {
    Serial.println("All updates successful. Rebooting...");
    ESP.restart();
  } else {
    Serial.println("Update failed.");
  }
  }

}




void SensorTask(void* parameter) {
  dht11.begin();

  while (true) {
    dht11.readValues();
   
   

    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  }
}





