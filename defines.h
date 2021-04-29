// Including libraries
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "AnalogInput.h"

// Defining directives
#define ESP_32_ADC_BITS 12
#define ESP_32_LOGIC_VOLTAGE 3.3
#define POTENTIOMETER_GPIO 12
#define BLE_DEVICE_NAME "ESP-32 OBDII"
#define SERVICE_UUID "0000180f-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00002a19-0000-1000-8000-00805f9b34fb"

// Object initialization
AnalogInput potentiometer(POTENTIOMETER_GPIO, ESP_32_ADC_BITS, ESP_32_LOGIC_VOLTAGE);
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

// Defining variables
bool deviceConnected = false;
bool oldDeviceConnected = false;
bool doOnce = false;
uint8_t sensorValue = 0;
uint8_t sensorRead = sensorValue;

// Server callbacks, extends BLEServerCallbacks class
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};