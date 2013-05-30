// (c) Copyright 2010-2012 Tappt.
// Released under Apache License, version 2.0
//
// Arduino Keg Server

#include <Arduino.h>
#include <SoftwareSerial.h>

#include <PN532.h>
#include <NFCLinkLayer.h>
#include <SNEP.h>
#include <NdefMessage.h>


#include <WiFly.h>
#include "RestClient.h"
#include "NFC.h"
#include "Debug.h"

#define SSID      "casanova"
#define PASSWORD       "bitchhunter"
#define AUTH      WIFLY_AUTH_WPA2_PSK

//#define LOGGING 1
#define KegeratorKey "Foobar 123"

#define MAX_NDEF_RECORDS 2

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

WiFly wifly(2, 3);
NFC nfc;
RestClient r(wifly, "192.168.1.139");

void setup()
{
	// initialize serial communications at 9600 bps:
	Serial.begin(9600); 
	
	Memory();

	wifly.reset();

	while (1) {
		if (wifly.join(SSID, PASSWORD, AUTH)) {
			DBGL("Succeed to join " SSID);
			wifly.clear();
			break;
		} else {
			DBGL("Failed to join " SSID);
			delay(1000);
		}
	}

	delay(3000);

	nfc.Setup();
}

void loop()
{
	Memory();
	
	char* message = (char*)nfc.Read();

	if(message == NULL)
	{
		DBGL("No DATA");
		return;
	}
	
	Memory();

	Serial.println(message);
	
	// Sleep for a second so they don't authorize twice.
	delay(1000);
	
	PourInfo* p = r.Validate(KegeratorKey, message);

	if (p == NULL)
	{
		return;
	}
	
	p->PouredOunces = 12 * 10;
	r.Pour(KegeratorKey, message, p);
}