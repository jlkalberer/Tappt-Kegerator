#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <OneWire.h>

// I need to disable CRC because it eats up a ton of memory

// You can exclude CRC checks altogether by defining this to 0
#define ONEWIRE_CRC 0

// Select the table-lookup method of computing the 8-bit CRC
// by setting this to 1.  The lookup table enlarges code size by
// about 250 bytes.  It does NOT consume RAM (but did in very
// old versions of OneWire).  If you disable this, a slower
// but very compact algorithm is used.
#define ONEWIRE_CRC8_TABLE 0

// You can allow 16-bit CRC checks by defining this to 1
// (Note that ONEWIRE_CRC must also be 1.)
#define ONEWIRE_CRC16 0

class Temperature 
{
public:
	Temperature(int pin);
	float GetTemperature();

private:
	OneWire sensor;
};

#endif