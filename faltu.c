#include <Arduino.h>
#include <Update.h>
#include <HTTPClient.h>
#include "WifiHandler.hpp"
#include "MQTTHandler.hpp"
#include "DHTSensor.hpp"

const String firmwareURL = "http://192.168.100.42:5000/api/get-firmware?file=main.ino.merged.bin";

bool fetchAndUpdate(const String& url) {
  HTTPClient http;
  http.begin(url);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    if (Update.begin(contentLength)) {
      size_t written = Update.writeStream(*stream);
      if (written == contentLength && Update.end() && Update.isFinished()) {
        Serial.println("Firmware update complete.");
        return true;
      }
    }
  }

  Serial.println("Firmware update failed.");
  return false;
}

// MQTT and WiFi setup
const char* mqttBroker = "broker.emqx.io";
const char* mqttTopic = "esp32/iot_temp";

MQTTHandler mqtt(mqttBroker, mqttTopic, 2);
WiFiHandler wifi("ESP32_AP", "password123", &mqtt);

// Function prototypes
void SensorTask(void* parameter);
void OTATask(void* parameter);

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
    1
  );

  // Create OTA check task on Core 0
  xTaskCreatePinnedToCore(
    OTATask,
    "OTA Task",
    10000,
    NULL,
    1,
    NULL,
    0
  );
}

void loop() {
  mqtt.loop();  // Keep MQTT alive
}

void SensorTask(void* parameter) {
  dht11.begin();

  while (true) {
    dht11.readValues();
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  }
}

void OTATask(void* parameter) {
  while (true) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Checking for firmware update...");
      bool success = fetchAndUpdate(firmwareURL);
      if (success) {
        Serial.println("Rebooting...");
        delay(1000);
        ESP.restart();
      }
    } else {
      Serial.println("WiFi not connected. Skipping OTA check.");
    }

    // Check every 10 minutes
    vTaskDelay(600000 / portTICK_PERIOD_MS); // 600000 ms = 10 mins
  }
}
