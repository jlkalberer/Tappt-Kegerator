#include "NFC.h"

#define BLOCK_SIZE 16

NFC::NFC() : nfc(SCK, MISO, MOSI, SS) , linkLayer(&nfc), snep(&linkLayer)
{
	this->state = ReadPhone;
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
	if (this->state == ReadCard)
	{
		this->state = ReadPhone;

		uint8_t success;
		uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
		uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

		// Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
		// 'uid' will be populated with the UID, and uidLength will indicate
		// if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
		success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

		if (success) {
			// Display some basic information about the card
			DBGL("Found an ISO14443A card");
			DBG("  UID Length: ");DBG(uidLength);DBGL(" bytes");
			DBG("  UID Value: ");
			DBGL("");

			if (uidLength == 4)
			{
				// We probably have a Mifare Classic card ... 
				DBGL("Seems to be a Mifare Classic card (4 byte UID)");

				// Now we need to try to authenticate it for read/write access
				// Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
				DBGL("Trying to authenticate block 4 with default KEYA value");
				uint8_t keya[6] = {(byte)0xD3,(byte)0xF7,(byte)0xD3,(byte)0xF7,(byte)0xD3,(byte)0xF7};
				// Start with block 4 (the first block of sector 1) since sector 0
				// contains the manufacturer data and it's probably better just
				// to leave it alone unless you know what you're doing

				int currentBlock = 4;
				uint8_t data[BLOCK_SIZE];
				uint8_t end = currentBlock + 8;
				uint8_t headerLength = 0;
				uint8_t contentLength = 0;
				uint8_t position = 0;

				for (currentBlock; currentBlock < end; currentBlock++)
				{
					if (currentBlock % 4 == 3)
					{
						continue;
					}

					if (currentBlock % 4 == 0) 
					{
						success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, currentBlock, 0, keya);
					}

					if (success)
					{
						// Try to read the contents of block 4
						success = nfc.mifareclassic_ReadDataBlock(currentBlock, data);

						if (success)
						{
							// do some validation to make sure this card is readable and get lengths of the data.
							// Here is a resource on the card http://learn.adafruit.com/adafruit-pn532-rfid-nfc/ndef
							if (currentBlock == 4)
							{
								// 0x03 means that it is reading an NDEF Message.
								if (data[2] != 0x03)
								{
									return NULL;
								}

								// This is how the chip get's written by android.
								headerLength = data[5];
								contentLength = data[6];

								uint8_t size = 8;
								memcpy(this->rxNDEFMessage, &data[7], size);
								position += size;
							}
							else
							{
								uint8_t size = BLOCK_SIZE;
								uint8_t contentLeft = (headerLength + contentLength) - position;
								if (contentLeft < BLOCK_SIZE)
								{
									size = contentLeft;
								}

								memcpy(&this->rxNDEFMessage[position], data, size);
								position += BLOCK_SIZE;
							}

							if (position >= headerLength + contentLength) 
							{
								break;
							}
						}
						else
						{
							DBGL("Ooops ... unable to read the requested block.  Try another key?");
						}
					}

					else
					{
						DBGL("Ooops ... authentication failed: Try another key?");
					}
				}

				this->rxNDEFMessage[headerLength + contentLength - 2] = '\0';
				this->rxNDEFMessagePtr = &this->rxNDEFMessage[headerLength];
			}

			// TODO - Support Mifare Ultralight
			else if (uidLength == 7)
			{
				// We probably have a Mifare Ultralight card ...
				DBGL("Seems to be a Mifare Ultralight tag (7 byte UID)");

				// Try to read the first general-purpose user page (#4)
				DBGL("Reading page 4");
				uint8_t data[32];
				success = nfc.mifareultralight_ReadPage (4, data);
				if (success)
				{
					// Data seems to have been read ... spit it out
					this->rxNDEFMessagePtr = NULL;

					// Wait a bit before reading the card again
					delay(1000);
				}
				else
				{
					DBGL("Ooops ... unable to read the requested page!?");
				} 
			}
		}
	}
	else if (this->state == ReadPhone)
	{
		this->state = ReadCard;
		
		DBGL();
		DBGL(F("---------------- LOOP ----------------------"));
		DBGL();

		uint32_t rxResult = GEN_ERROR; 
		rxNDEFMessagePtr = &rxNDEFMessage[0];

		memset((void*)rxNDEFMessage, '\0', sizeof(rxNDEFMessage));

		rxResult = snep.rxNDEFPayload(rxNDEFMessagePtr);

		if (rxResult == SEND_COMMAND_RX_TIMEOUT_ERROR)
		{
			DBGL("..rxNDEFPayload() timeout");
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
				DBGL("BLEH");
				this->rxNDEFMessagePtr = NULL;
				return NULL;
			}

			NdefRecord record = message->getRecord(0);

			length = record.getPayloadLength();

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
			this->rxNDEFMessagePtr = NULL;
		}
	}

	return rxNDEFMessagePtr;
}