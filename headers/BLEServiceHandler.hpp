#ifndef BLE_SERVICE_HANDLER_HPP
#define BLE_SERVICE_HANDLER_HPP

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "JsonParser.hpp"
// #include "WiFiHandler.hpp"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyCharacteristicCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic) override;

private:
  String ssid;
  String password;
};

class BLEServiceHandler
{
public:
  BLEServiceHandler(const char *deviceName);
  void init();

private:
  const char *_deviceName;
  BLEServer *pServer;
  BLEService *pService;
  BLECharacteristic *pCharacteristic;
};

#endif // BLE_SERVICE_HANDLER_HPP
