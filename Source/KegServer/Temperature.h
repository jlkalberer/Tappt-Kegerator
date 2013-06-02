#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <OneWire.h>

class Temperature 
{
public:
	Temperature(int pin);
	float GetTemperature();

private:
	OneWire sensor;
};

#endif