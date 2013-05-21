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
	uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    Serial.println("");
    
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      //uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      //uint8_t keya[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	  uint8_t keya[6] = {(byte)0xD3,(byte)0xF7,(byte)0xD3,(byte)0xF7,(byte)0xD3,(byte)0xF7};
	  // Start with block 4 (the first block of sector 1) since sector 0
	  // contains the manufacturer data and it's probably better just
	  // to leave it alone unless you know what you're doing
	  int key = 12;
	  
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, key, 0, keya);
	  
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];

        // If you want to write something to block 4 to test with, uncomment
		// the following line and this text should be read back in a minute
        // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
        // success = nfc.mifareclassic_WriteDataBlock (4, data);

		// When reading the message in block 4 
		//	04:05 is the content-type length;
		//	04:06 is the length of the content

		// 1. get the content type length and content length
		// 2. validate content type
		// 3. get message only by and use the content length 
		// 7. Skip block 7 as it is just holding some garbage :)
		// 8. Block 8 has no header info :D

		int end = key + 4;
		for (key; key < end; key++)
		{
			// Try to read the contents of block 4
			success = nfc.mifareclassic_ReadDataBlock(key, data);

			if (success)
			{
			  // Data seems to have been read ... spit it out
			  
			 // nfc.PrintHexChar(data, 16);
			  for (int i = 0; i < 16; i++) 
			  {
				Serial.print(data[i], 16);
				Serial.print(":");
			  }

			  // Wait a bit before reading the card again
			}
			else
			{
			  Serial.println("Ooops ... unable to read the requested block.  Try another key?");
			}
		}
		delay(1000);
		
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }
    
    if (uidLength == 7)
    {
      // We probably have a Mifare Ultralight card ...
      Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");

      // Try to read the first general-purpose user page (#4)
      Serial.println("Reading page 4");
      uint8_t data[32];
      success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        Serial.println("");

        // Wait a bit before reading the card again
        delay(1000);
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested page!?");
      } 
    }
  }
}
/*{
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
		rxNDEFMessagePtr = NULL;
	} 
	else if (IS_ERROR(rxResult)) 
	{
		DBGL("rxNDEFPlayload() failed");
		rxNDEFMessagePtr = NULL;
	}
	else if (RESULT_OK(rxResult))
	{
		NdefMessage *message = new NdefMessage(rxNDEFMessagePtr, rxResult);
		
		int length = message->getRecordCount();

		if (length <= 0)
		{
			return NULL;
		}
		
		NdefRecord record = message->getRecord(0);

		length = record.getPayloadLength();

		DBG("L ");
		DBGL(length);

		if (length <= 0)
		{
			return (uint8_t*)NULL;
		}

		record.getPayload(rxNDEFMessage);

		// Remove the quotes
		rxNDEFMessagePtr = &rxNDEFMessage[1];
		rxNDEFMessage[length - 1] = '\0';

		delete message;
	}
	else
	{
		DBGL("Error...");
	}

	return rxNDEFMessagePtr;
}*/