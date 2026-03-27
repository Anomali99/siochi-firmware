#include "Connectivity.h"

Connectivity* pSiochi = NULL;

class Security : public BLESecurityCallbacks {
  void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) {
    if (cmpl.success) {
      Serial.println(">> Otentikasi Berhasil (Pairing Sukses)");
    } else {
      Serial.println(">> Otentikasi Gagal");
      Serial.printf("Fail reason: %d\n", cmpl.fail_reason);
    }
  }

  bool onConfirmPIN(uint32_t pin) { return true; }
  uint32_t onPassKeyRequest() { return 0; }
  void onPassKeyNotify(uint32_t pass_key) {}
  bool onSecurityRequest() { return true; }
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    if (pSiochi) {
      pSiochi->deviceConnected = true;
      Serial.println(">> BLE Device Connected");
    }
  };

  void onDisconnect(BLEServer* pServer) {
    if (pSiochi) {
      pSiochi->deviceConnected = false;
      Serial.println(">> BLE Device Disconnected");
    }
  }
};

class Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    String rxValue = String(pCharacteristic->getValue().c_str());
    if (rxValue.length() > 0 && pSiochi) {
      pSiochi->incomingConfig = rxValue;
      pSiochi->newConfigReceived = true;
    }
  }
};

Connectivity::Connectivity() {
  deviceConnected = false;
  oldDeviceConnected = false;
  newConfigReceived = false;
  hasNewNotification = false;
  wifiEnabled = true;
  pSiochi = this;
}

void Connectivity::setup() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("Status Awal: WiFi OFF");

  Serial.println("Menginisialisasi BLE...");

  BLEDevice::init(DEVICE_NAME);
  BLEDevice::setMTU(256);

  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT_NO_MITM);
  BLEDevice::setSecurityCallbacks(new Security());

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);

  pTxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_TX,
      BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic* pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  pRxCharacteristic->setCallbacks(new Callbacks());

  pService->start();
  pServer->getAdvertising()->start();

  BLESecurity* pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
  pSecurity->setCapability(ESP_IO_CAP_NONE);
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  pSecurity->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

  Serial.println("=== Siochi Siap (Menunggu Koneksi BLE) ===");
}

void Connectivity::notifyStatus(String status) {
  if (deviceConnected) {
    pTxCharacteristic->setValue(status.c_str());
    pTxCharacteristic->notify();
    Serial.println(">> BLE Sent: " + status);
  }
}

void Connectivity::parseConfig(String data) {
  if (data.startsWith("S1|")) {
    String payload = data.substring(3);
    if (payload == "BLE_ONLY") {
      wifiEnabled = false;
      Serial.println(">> Pengaturan: Mode BLE ONLY (WiFi OFF)");
    } else {
      wifiEnabled = true;
      int p1 = payload.indexOf('|');
      int p2 = payload.indexOf('|', p1 + 1);
      if (p1 > 0 && p2 > 0) {
        targetSSID = payload.substring(0, p1);
        targetPass = payload.substring(p1 + 1, p2);
        controllerIP = payload.substring(p2 + 1);
        controllerIP.trim();
        Serial.println(">> Pengaturan: Mode WiFi (" + targetSSID + ")");
      }
    }
  } else if (data.startsWith("S2|")) {
    // S2|Mode|WA|SMS|CALL (e.g., S2|0|1|0|1)
    int p1 = data.indexOf('|', 3);
    int p2 = data.indexOf('|', p1 + 1);
    int p3 = data.indexOf('|', p2 + 1);

    if (p1 > 0 && p2 > 0 && p3 > 0) {
      notifMode = data.substring(3, p1).toInt();
      notifWaOn = data.substring(p1 + 1, p2) == "1";
      notifSmsOn = data.substring(p2 + 1, p3) == "1";
      notifCallOn = data.substring(p3 + 1) == "1";
      Serial.printf(">> Pengaturan Notif: Mode=%d, WA=%d, SMS=%d, CALL=%d\n", notifMode, notifWaOn, notifSmsOn, notifCallOn);
    }
  } else if (data.startsWith("S3|")) {
    navMode = data.substring(3).toInt();
    Serial.printf(">> Pengaturan Navigasi: Mode=%d\n", navMode);
  } else if (data.startsWith("N1|") || data.startsWith("N2|") || data.startsWith("N3|")) {
    String type = data.substring(0, 2);

    if (type == "N1" && !notifWaOn) return;
    if (type == "N2" && !notifSmsOn) return;
    if (type == "N3" && !notifCallOn) return;

    String payload = data.substring(3);
    int p1 = payload.indexOf('|');
    if (p1 > 0) {
      notifType = type;
      notifSender = payload.substring(0, p1);
      notifMessage = payload.substring(p1 + 1);
      hasNewNotification = true;
      Serial.println(">> NOTIF MASUK [" + type + "] " + notifSender + ": " + notifMessage);
    }
  }
}

bool Connectivity::pingMobileApp() {
  Serial.print("Pinging Mobile App di ");
  Serial.print(controllerIP);
  Serial.print(":");
  Serial.println(MOBILE_APP_PORT);

  WiFiClient client;
  if (client.connect(controllerIP.c_str(), MOBILE_APP_PORT)) {
    Serial.println("Ping Sukses! TCP Handshake Berhasil.");
    client.println("HELLO_SIOCHI_HANDSHAKE");
    client.stop();
    return true;
  }

  String gatewayIP = WiFi.gatewayIP().toString();
  Serial.print("Mencoba Ping ke IP Gateway (Hotspot): ");
  Serial.print(gatewayIP);
  Serial.print(":");
  Serial.println(MOBILE_APP_PORT);

  if (client.connect(gatewayIP.c_str(), MOBILE_APP_PORT)) {
    Serial.println("Ping Sukses! TCP Handshake ke IP Gateway Berhasil.");
    client.println("HELLO_SIOCHI_HANDSHAKE");
    client.stop();
    return true;
  }

  Serial.println("Ping Gagal. Tidak ada respon di port " + String(MOBILE_APP_PORT));
  return false;
}

void Connectivity::connectToWiFi() {
  notifyStatus("STATUS:CONNECTING_WIFI");
  delay(500);

  Serial.println("Menghidupkan Radio WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(DEVICE_NAME);
  delay(100);

  WiFi.disconnect(true);
  delay(100);

  Serial.print("Menghubungkan ke ");
  Serial.println(targetSSID);

  if (targetPass.length() > 0) {
    WiFi.begin(targetSSID.c_str(), targetPass.c_str());
  } else {
    WiFi.begin(targetSSID.c_str());
  }

  int retry = 0;
  bool ledState = false;
  while (WiFi.status() != WL_CONNECTED && retry < 40) {
    delay(500);
    Serial.print(".");
    ledState = !ledState;
    retry++;
  }
  Serial.println("");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected! IP: " + WiFi.localIP().toString());

    notifyStatus("STATUS:WIFI_OK|" + WiFi.localIP().toString());

    delay(1000);
    notifyStatus("STATUS:PINGING_APP");

    if (pingMobileApp()) {
      notifyStatus("STATUS:APP_CONNECTED");
    } else {
      notifyStatus("STATUS:APP_UNREACHABLE");
    }
  } else {
    Serial.println("WiFi Gagal Terhubung.");
    notifyStatus("STATUS:WIFI_FAILED");
  }
}

void Connectivity::loop() {
  if (newConfigReceived) {
    newConfigReceived = false;
    parseConfig(incomingConfig);
    if (incomingConfig.startsWith("S1|") && wifiEnabled && targetSSID.length() > 0) {
      connectToWiFi();
    }
  }

  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    pServer->startAdvertising();
    Serial.println("Restart Advertising...");
    oldDeviceConnected = deviceConnected;
  }
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}