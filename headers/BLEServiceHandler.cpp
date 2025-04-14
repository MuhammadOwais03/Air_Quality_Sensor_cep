#include "BLEServiceHandler.hpp"
#include <Arduino.h>
#include <WiFi.h>

// WiFiProvider wifi_handler;
bool shouldStopBLE = false;

void MyCharacteristicCallback::onWrite(BLECharacteristic *pCharacteristic)
{
  String value = pCharacteristic->getValue();

  Serial.print("Received value: ");
  for (char c : value)
  {
    Serial.print(c);
  }
  Serial.println();
  if (!value.isEmpty())
  {
    Serial.println("Received over BLE: " + String(value.c_str()));
    JsonParser parser(String(value.c_str()));
    parser.getJsonValues("ssid", "password");
    ssid = parser.ssid;
    password = parser.password;
    Serial.print("Parsed SSID: ");
    Serial.println(ssid);
    Serial.print("Parsed Password: ");
    Serial.println(password);
    shouldStopBLE = true;
    WiFi.begin(ssid.c_str(), password.c_str());

    int maxAttempts = 20;
    while (WiFi.status() != WL_CONNECTED && maxAttempts-- > 0)
    {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("\nWiFi connected successfully!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    }
    else
    {
      Serial.println("\nFailed to connect to WiFi.");
    }
  }
}

BLEServiceHandler::BLEServiceHandler(const char *deviceName)
    : _deviceName(deviceName), pServer(nullptr), pService(nullptr), pCharacteristic(nullptr) {}

void BLEServiceHandler::init()
{
  BLEDevice::init(_deviceName);

  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue("Hello from ESP32!");
  pCharacteristic->setCallbacks(new MyCharacteristicCallback());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("BLE service started and advertising...");
}
