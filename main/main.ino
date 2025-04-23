#include <Arduino.h>
#include <Update.h>
#include <HTTPClient.h>
#include "WifiHandler.hpp"
#include "MQTTHandler.hpp"
#include "DHTSensor.hpp"

const String firmwareURL = "https://air-quality-backend-rho.vercel.app/api/upload/get-firmware";

// Global variable to track OTA status
bool otaRequested = false;

bool fetchAndUpdate(const String& url) {
  HTTPClient http;
  
  // Start the HTTP connection
  http.begin(url);

  int httpCode = http.GET();
  Serial.println("HTTP GET code: " + String(httpCode));

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    Serial.println("Content Length: " + String(contentLength));

    if (contentLength > 0) {
      if (Update.begin(contentLength)) {
        size_t written = Update.writeStream(*stream);
        Serial.println("Written bytes: " + String(written));

        if (written == contentLength && Update.end() && Update.isFinished()) {
          Serial.println("Firmware update complete.");
          return true;
        } else {
          Serial.println("Error during the firmware update process.");
        }
      } else {
        Serial.println("Update.begin() failed.");
      }
    } else {
      Serial.println("Invalid content length received.");
    }
  } else {
    Serial.println("HTTP GET failed with error code: " + String(httpCode));
  }

  Serial.println("Firmware update failed.");
  return false;
}

// MQTT and WiFi setup
const char* mqttBroker = "broker.emqx.io";
const char* mqttTopic = "esp32/iot_temp";
const char* mqttOTATopic = "esp32/ota";

MQTTHandler mqtt(mqttBroker, mqttTopic, 2);
MQTTHandler mqtt1(mqttBroker, mqttOTATopic, 2);
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
  mqtt1.setupMQTT();

  // Set the OTA callback to set otaRequested flag
  mqtt1.setOTACallback([]() {
    otaRequested = true; // Set the flag when OTA is triggered
  });

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
  mqtt1.loop();

  // Check if OTA is requested and trigger OTA task
  if (otaRequested) {
    otaRequested = false;  // Reset the flag
    // Trigger OTA task here (or do it directly in a task)
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
}

void SensorTask(void* parameter) {
  dht11.begin();

  while (true) {
    dht11.readValues();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void OTATask(void* parameter) {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Checking for firmware update...");
    bool success = fetchAndUpdate(firmwareURL);
    Serial.println(success);
    if (success) {
      Serial.println("Rebooting...");
      delay(1000);
      ESP.restart();
    }
  } else {
    Serial.println("WiFi not connected. Skipping OTA check.");
  }

  vTaskDelete(NULL); // Delete the task when done
}
