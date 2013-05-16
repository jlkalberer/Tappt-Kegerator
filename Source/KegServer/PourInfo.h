#ifndef POUR_INFO_H
#define POUR_INFO_H

// Class for holding the information on pours.
struct PourInfo
{
    // Gets or sets the available ounces the user can pour.
	uint16_t AvailableOunces;

    // Gets or sets the poured ounces.
	uint16_t PouredOunces;

	// Gets or sets the pour key.
    char* PourKey;
};

#endif