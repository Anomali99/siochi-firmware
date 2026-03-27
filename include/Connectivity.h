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
  String targetSSID;
  String targetPass;
  String controllerIP;
  String connectionMode;
  bool oldDeviceConnected;
  void parseConfig(String data);
  void connectToWiFi();
  bool pingMobileApp();

 public:
  String incomingConfig;
  String notifType;
  String notifSender;
  String notifMessage;
  bool deviceConnected;
  bool newConfigReceived;
  bool wifiEnabled;
  bool notifWaOn;
  bool notifSmsOn;
  bool notifCallOn;
  bool hasNewNotification;
  int notifMode;
  int navMode;
  Connectivity();
  void setup();
  void loop();
  void notifyStatus(String status);
};