#include "NFC.h"

NFC::NFC() : nfc(SCK, MISO, MOSI, SS) , linkLayer(&nfc), snep(&linkLayer)
{
}

void NFC::Setup()
{
	nfc.initializeReader();

	uint32_t versiondata = nfc.getFirmwareVersion();
	if (! versiondata) {
		DBG("Didn't find PN53x board");
		while (1); // halt
	}
	// Got ok data, print it out!
	DBG("Found chip PN5"); DBGBL((versiondata>>24) & 0xFF, HEX);
	DBG("Firmware ver. "); DBGB((versiondata>>16) & 0xFF, DEC);
	DBG('.'); DBGBL((versiondata>>8) & 0xFF, DEC);
	DBG("Supports "); DBGBL(versiondata & 0xFF, HEX);

	// configure board to read RFID tags and cards
	nfc.SAMConfig();

	nfc.configurePeerAsTarget(SNEP_SERVER);
}

uint8_t* NFC::Read()
{
	DBGL();
	DBGL(F("---------------- LOOP ----------------------"));
	DBGL();

	uint32_t rxResult = GEN_ERROR; 
	rxNDEFMessagePtr = &rxNDEFMessage[0];

	memset((void*)rxNDEFMessage, '\0', sizeof(rxNDEFMessage));

	rxResult = snep.rxNDEFPayload(rxNDEFMessagePtr);

	if (rxResult == SEND_COMMAND_RX_TIMEOUT_ERROR)
	{
		DBGL("rxNDEFPayload() timeout");
		rxNDEFMessagePtr = (uint8_t*)NULL;
	} 
	else if (IS_ERROR(rxResult)) 
	{
		DBGL("rxNDEFPlayload() failed");
		rxNDEFMessagePtr = (uint8_t*)NULL;
	}
	else if (RESULT_OK(rxResult))
	{
		NdefMessage *message = new NdefMessage(rxNDEFMessagePtr, rxResult);
		int length = message->getRecordCount();

		DBG("NDEF record: ");
		DBGL(length);
		
		if (length <= 0)
		{
			return (uint8_t*)NULL;
		}
		
		NdefRecord record = message->getRecord(0);

		length = record.getPayloadLength();

		if (length <= 0)
		{
			return (uint8_t*)NULL;
		}

		record.print();

		record.getPayload(rxNDEFMessage);

		// Remove the quotes
		rxNDEFMessagePtr = &rxNDEFMessage[1];
		rxNDEFMessage[length - 1] = '\0';

		delete message;
	}

	return rxNDEFMessagePtr;
}