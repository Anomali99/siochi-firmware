#pragma once

#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <WiFi.h>

#include "Config.h"

class Connectivity {
 private:
  BLEServer* pServer;
  BLECharacteristic* pTxCharacteristic;
  bool oldDeviceConnected;

  String targetSSID;
  String targetPass;
  String controllerIP;
  String connectionMode;

  void parseConfig(String data);
  void connectToWiFi();
  bool pingMobileApp();
  void setLed(bool on);

 public:
  Connectivity();
  void setup();
  void loop();
  void notifyStatus(String status);

  bool deviceConnected;
  bool newConfigReceived;
  String incomingConfig;
};