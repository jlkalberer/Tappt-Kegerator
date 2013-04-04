#ifndef POUR_INFO_H
#define POUR_INFO_H

// Class for holding the information on pours.
struct PourInfo
{
    // Gets or sets the available ounces the user can pour.
    float AvailableOunces;

    // Gets or sets the pour key.
    const char* PourKey;

    // Gets or sets the unique id. This is generated simply for verifying the pour key.
    int UniqueID;

    // Gets or sets the poured ounces.
    float PouredOunces;
};

#endif