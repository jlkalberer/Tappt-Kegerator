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

#include <OneWire.h>
#include "Temperature.h"

#define SSID      "casanova"
#define PASSWORD       "bitchhunter"
#define AUTH      WIFLY_AUTH_WPA2_PSK

//#define LOGGING 1
#define KegeratorKey "Foobar 123"

#define MAX_NDEF_RECORDS 2

// Setup the pins
#define WIFLY_PIN_1 3
#define WIFLY_PIN_2 4
#define SOLENOID 5
#define FLOW 0 // Actually pin 2 but this is the first Interrupt pin
#define TEMP 6
/*
#define SCK 13
#define MOSI 11
#define SS 10
#define MISO 12
*/

#define LISTENING 1
#define POURING 2
#define DONE_POURING 3
#define PULSE_EPSILON 0 // In case some pulses occur because of pressure differences between each side of the solenoid

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

WiFly wifly(WIFLY_PIN_1, WIFLY_PIN_2);
NFC nfc;
RestClient r(wifly, "192.168.1.144");

byte state, waitCount;
volatile int flowCount = 0;
unsigned long time, totalPulses;

void setup()
{
	Serial.begin(9600); 

	// initialize serial communications at 9600 bps:
	
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

	delay(200);

	nfc.Setup();

	pinMode(SOLENOID, OUTPUT);
	digitalWrite(SOLENOID, LOW);

	state = LISTENING;
}

char* message = NULL;
PourInfo* p = NULL;

void loop()
{
	Memory();
	ReadTemperature();

	switch(state)
	{
		case LISTENING:
		{
			message = (char*)nfc.Read();

			if(message == NULL)
			{
				break;
			}
			Serial.println(message);
	
			// Sleep for a second so they don't authorize twice.
	
			p = r.Validate(KegeratorKey, message);

			if (p == NULL)
			{
				break;
			}

			state = POURING;
			waitCount = 0;
			digitalWrite(SOLENOID, HIGH);
			delay(200);
			attachInterrupt(FLOW, flowCounter, FALLING);
			
			message = NULL;

			break;
		}
		case POURING:
		{
			if ((millis() - time) >= 1000) 
			{
				detachInterrupt(FLOW);
				time = millis();
				
				if (flowCount == PULSE_EPSILON)
				{
					waitCount++;
					int maxWait = totalPulses > PULSE_EPSILON ? 3 : 5;
					if (waitCount > maxWait)
					{
						state = DONE_POURING;
					}
					
				}
				else 
				{
					waitCount = 0;
					//Serial.print("P:");
					//Serial.println(flowCount);
					totalPulses += flowCount;
					flowCount = 0;
				}
				attachInterrupt(FLOW, flowCounter, FALLING);
			}

			break;
		}
		case DONE_POURING:
		{
			waitCount = 0;
			detachInterrupt(FLOW);
			digitalWrite(SOLENOID, LOW);

			if (totalPulses == 0)
			{
				state = LISTENING;
				break;
			}

			p->PouredOunces = totalPulses;      //liter
			Serial.print("P: ");
			Serial.println(p->PouredOunces);
			
			r.Pour(KegeratorKey, p);

			state = LISTENING;

			totalPulses = 0;

			delay(2000);

			break;
		}
		default:
			state = LISTENING;
			break;
	}
}

void ReadTemperature()
{
	return;
	Temperature * temp = new Temperature(TEMP);
	float t = temp->GetTemperature();
	//Serial.print("T:");
	//Serial.println(t);
	delete temp;
}

void flowCounter() 
{
	flowCount++;
}