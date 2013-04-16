#ifndef POUR_INFO_H
#define POUR_INFO_H

// Class for holding the information on pours.
struct PourInfo
{
    // Gets or sets the available ounces the user can pour.
    int AvailableOunces;

    // Gets or sets the pour key.
    char* PourKey;

    // Gets or sets the unique id. This is generated simply for verifying the pour key.
    unsigned int UniqueID;

    // Gets or sets the poured ounces.
    unsigned int PouredOunces;
};

#endif