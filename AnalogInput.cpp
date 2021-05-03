#include "AnalogInput.h"

AnalogInput::AnalogInput(byte pin, byte adcBits, float logicVoltage) { 

this->pin = pin; 
this->adcBits = adcBits; 
this->logicVoltage = logicVoltage;

}

/**
 * Read analog value from board ADC
 */
int AnalogInput::read() {
	return analogRead(pin);
}

float AnalogInput::voltage() {
  return read() * (logicVoltage / (pow(2, adcBits) - 1));
}

byte AnalogInput::percent() {
  return (read() / (pow(2, adcBits) - 1)) * 100;
}
