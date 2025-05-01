#include <Arduino.h>
#include <Update.h>
#include <HTTPClient.h>
#include "WifiHandler.hpp"
#include "MQTTHandler.hpp"
#include "DHTSensor.hpp"
#include <LiquidCrystal_I2C.h>

const String firmwareURL = "https://air-quality-backend-rho.vercel.app/api/upload/get-firmware";
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Global variable to track OTA status
bool otaRequested = false;
bool otaUpdated = false;

// bool fetchAndUpdate(const String& url) {
//   HTTPClient http;
  
//   // Start the HTTP connection
//   http.begin(url);

//   int httpCode = http.GET();
//   Serial.println("HTTP GET code: " + String(httpCode));

//   if (httpCode == HTTP_CODE_OK) {
//     int contentLength = http.getSize();
//     WiFiClient* stream = http.getStreamPtr();

//     Serial.println("Content Length: " + String(contentLength));

//     if (contentLength > 0) {
//       if (Update.begin(contentLength)) {
//         size_t written = Update.writeStream(*stream);
//         Serial.println("Written bytes: " + String(written));

//         if (written == contentLength && Update.end() && Update.isFinished()) {
//           Serial.println("Firmware update complete.");
//           lcd.clear();
//           lcd.setCursor(0, 0);
//           lcd.print("Firmware Update Completed");

//           return true;
//         } else {
//           Serial.println("Error during the firmware update process.");
//         }
//       } else {
//         Serial.println("Update.begin() failed.");
//       }
//     } else {
//       Serial.println("Invalid content length received.");
//     }
//   } else {
//     Serial.println("HTTP GET failed with error code: " + String(httpCode));
//   }

//   Serial.println("Firmware update failed.");
//   return false;
// }

bool fetchAndUpdate(const String& url) {
  HTTPClient http;
  http.begin(url);

  int httpCode = http.GET();
  Serial.println("HTTP GET code: " + String(httpCode));

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    Serial.println("Content Length: " + String(contentLength));

    if (contentLength > 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Updating Firmware");

      if (Update.begin(contentLength)) {
        size_t written = Update.writeStream(*stream);
        Serial.println("Written bytes: " + String(written));

        if (written == contentLength) {
          if (Update.end() && Update.isFinished()) {
              otaUpdated = true;

            Serial.println("Firmware update complete.");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Update Completed");
            return true;
          } else {
            otaUpdated = false;
            Serial.println("Update.end() failed.");
            Serial.print("Error #: ");
            Serial.println(Update.getError());
          }
        } else {
          otaUpdated = false;
          Serial.println("Incomplete update written.");
        }
      } else {
        otaUpdated = false;
        Serial.println("Update.begin() failed.");
        Serial.print("Error #: ");
        Serial.println(Update.getError());
      }
    } else {
      otaUpdated = false;
      Serial.println("Invalid content length received.");
    }
  } else {
    otaUpdated = false;
    Serial.println("HTTP GET failed with error code: " + String(httpCode));
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Update Failed");
  Serial.println("Firmware update failed.");
  return false;
}



// MQTT and WiFi setup
const char* mqttBroker = "broker.emqx.io";
const char* mqttTopic = "esp32/iot_temp";
const char* mqttOTATopic = "esp32/ota";
const char* mqttControlTopic = "esp32/control";

MQTTHandler mqtt(mqttBroker, mqttTopic, 2);
MQTTHandler mqtt1(mqttBroker, mqttOTATopic, 2);
MQTTHandler mqtt2(mqttBroker, mqttControlTopic, 2);
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
  mqtt2.setupMQTT();

  // Set the OTA callback to set otaRequested flag
  mqtt1.setOTACallback([]() {
    otaRequested = true; // Set the flag when OTA is triggered
  });

  Wire.begin(21, 22);  // ESP32 I2C: SDA = 21, SCL = 22
  lcd.init();
  lcd.backlight();

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
  mqtt2.loop();

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

    lcd.clear();
  while (true) {
    bool success = dht11.readValues();

  if (success && !otaUpdated) {
    float temp = dht11.getTemperature();
    float hum = dht11.getHumidity();

    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print((char)223); // degree symbol
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(hum);
    lcd.print("%");
  } else if (!otaUpdated) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor error!");
  }
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


void goToSleepFor30Seconds() {
  Serial.println("Going to sleep for 30 seconds...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sleeping...");

  delay(1000);  // Allow message to show before sleeping
  esp_sleep_enable_timer_wakeup(30 * 1000000); // 30 seconds in microseconds
  esp_deep_sleep_start();
}

