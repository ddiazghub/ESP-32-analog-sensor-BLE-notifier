#ifndef ANALOG_INPUT_H
#define ANALOG_INPUT_H
#include <Arduino.h>

class AnalogInput {

  private:
		byte pin;
    byte adcBits;
    float logicVoltage;
    float power(float number, float exponent);

	public:
		AnalogInput(byte pin, byte adcBits, float logicVoltage);
		int read();
    float voltage();
    byte percent();

};
#endif