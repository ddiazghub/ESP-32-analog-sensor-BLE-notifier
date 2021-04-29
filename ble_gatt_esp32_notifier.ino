#include "defines.h"

// Setup
void setup() {

  Serial.begin(115200);
  sensorValue = (uint8_t) potentiometer.percent();

  // Creation of BLE device and Gatt Server.
  BLEDevice::init(BLE_DEVICE_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  Serial.print("\nBLE server ");
  Serial.print(BLE_DEVICE_NAME);
  Serial.println(" created.");

  // Creation of BLED service and characteristic to advertise.
  BLEService *pService = pServer->createService(SERVICE_UUID);
  Serial.print("New service ");
  Serial.print(SERVICE_UUID);
  Serial.println(" created.");

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  Serial.print("New characteristic ");
  Serial.print(CHARACTERISTIC_UUID);
  Serial.println(" created.");

  // Creation of a descriptor for the created characteristic.
  pCharacteristic->addDescriptor(new BLE2902());

  // Starting the service and advertising.
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // Not being advertised
  BLEDevice::startAdvertising();
  Serial.println("Advertising. Waiting for a client connection to start notifications...");
}

void loop() {

  // notify changed value
  if (deviceConnected) {
    sensorRead = (uint8_t) potentiometer.percent();
    if (doOnce) {
      delay(2000);
      notify();
      doOnce = false;
    }
    boolean sensorValueChanged = sensorRead != sensorValue;
    if (sensorValueChanged) {
      notify();
    }
    delay(1000);
  }

  // Client disconnected.
  if (!deviceConnected && oldDeviceConnected) {
    Serial.println("Client disconnected. The device will start to advertise once again.");
    delay(1000);
    pServer->startAdvertising();
    Serial.println("\nAdvertising. Waiting for a client connection to start notifications...");
    oldDeviceConnected = deviceConnected;
  }

  // Client connected.
  if (deviceConnected && !oldDeviceConnected) {
    Serial.println("A client just connected. Starting notifications.\n");
    oldDeviceConnected = deviceConnected;
    doOnce = true;
  }
}

// Notify client.
void notify() {
  Serial.print("Gasoline level: ");
  Serial.print(sensorRead);
  Serial.println("%");
  pCharacteristic->setValue((uint8_t*)&sensorRead, 1);
  pCharacteristic->notify();
  sensorValue = sensorRead;
}
