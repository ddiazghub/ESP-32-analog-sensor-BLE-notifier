#include "defines.h"

// Setup
void setup() {

  Serial.begin(115200);
  sensorValue = (uint8_t) potentiometer.percent();

  // Start LCD display.
  display.init();
  display.backlight();

  // Creation of BLE device and Gatt Server.
  BLEDevice::init(BLE_DEVICE_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  Serial.print("\nBLE server ");
  Serial.print(BLE_DEVICE_NAME);
  Serial.println(" created.");

  printTodisplay("   BLE Server   ", 0, 0);
  printTodisplay("  ", 0, 1);
  printTodisplay(BLE_DEVICE_NAME, 2, 1);

  // Creation of BLE service and characteristic to advertise.
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

  delay(2000);

  // Starting the service and advertising.
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // Not being advertised
  BLEDevice::startAdvertising();
  Serial.println("Advertising. Waiting for a client connection to start notifications...");

  display.clear();
  printTodisplay("  Advertising.  ", 0, 0);
  printTodisplay("Waiting for client", 0, 0);
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
  } else {
    printTodisplay("  Advertising.  ", 0, 0);
    scrollText("Waiting for a client connection to start notifications...", 1, 250, 16);
  }

  // Client disconnected.
  if (!deviceConnected && oldDeviceConnected) {
    Serial.println("Client disconnected. The device will start to advertise once again.");
    printTodisplay("     Client     ", 0, 0);
    printTodisplay("  Disconnected  ", 0, 1);
    delay(1000);
    pServer->startAdvertising();
    Serial.println("\nAdvertising. Waiting for a client connection to start notifications...");
    oldDeviceConnected = deviceConnected;
  }

  // Client connected.
  if (deviceConnected && !oldDeviceConnected) {
    Serial.println("A client just connected. Starting notifications.\n");
    printTodisplay("     Client     ", 0, 0);
    printTodisplay("   Connected    ", 0, 1);
    delay(1000);
    oldDeviceConnected = deviceConnected;
    doOnce = true;
  }

}

// Notify client.
void notify() {
  Serial.print("Notified new gasoline level to client: ");
  Serial.print(sensorRead);
  Serial.println("%");
  display.clear();
  printTodisplay("  Notifying...  ", 0, 0);
  printTodisplay(" Gasoline: ", 0, 1);
  printTodisplay(String(sensorRead), 11, 1);
  if (sensorRead < 10) {
    printTodisplay("%", 12, 1);
  } else if (sensorRead < 100) {
    printTodisplay("%", 13, 1);
  } else {
    printTodisplay("%", 14, 1);
  }
  pCharacteristic->setValue((uint8_t*)&sensorRead, 1);
  pCharacteristic->notify();
  sensorValue = sensorRead;
}

void printTodisplay(String text, byte column, byte row) {
  display.setCursor(column, row);
  display.print(text);
}

void scrollText(String message, byte row, int delayTime, byte lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    printTodisplay(message.substring(pos, pos + lcdColumns), 0, row);
    delay(delayTime);

    if (deviceConnected && !oldDeviceConnected) {
     return;
    }
  }
}