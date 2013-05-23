#ifndef NFC_H
#define NFC_H

#include <PN532.h>
#include <NFCLinkLayer.h>
#include <SNEP.h>

#include <NdefMessage.h>

#include "Debug.h"

#define SCK 13
#define MOSI 11
#define SS 10
#define MISO 12

// This message shall be used to rx or tx 
// NDEF messages it shall never be released
#define MAX_PKT_HEADER_SIZE  50
#define MAX_PKT_PAYLOAD_SIZE 100
#define PACKET_SIZE 70

enum NFCState
{
	ReadCard,
	ReadPhone
};

class NFC
{
public:
	// Initialize the private members
	NFC();

	// Sets up the NFC device by initializing it to accept NDEF payloads.
	void Setup();

	// Used to read and see if there are any NDEF messages
	uint8_t* Read();
private:
	// The message buffer
	uint8_t rxNDEFMessage[PACKET_SIZE];
	uint8_t *rxNDEFMessagePtr; 
	
	// NFC NDEF 
	PN532 nfc;
	NFCLinkLayer linkLayer;
	SNEP snep;

	NFCState state;
};

#endif