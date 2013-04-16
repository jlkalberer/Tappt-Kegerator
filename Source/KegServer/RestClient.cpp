#include "RestClient.h"
#include "Debug.h"
#include "MemoryFree.h"


#define PourResource "/api/pour"

// The kegerator communication key.
#define KegeratorCommunicationKey "Kegerator-Communication-Key"
#define CONTENT_LENGTH "Content-Length"

RestClient::RestClient(WiFly& wifly, const char* uri)
	: client(wifly)
{
	this->uri = uri;
	//this->currentResponse = NULL;
	this->token_list = create_token_list(9);
}

void RestClient::Setup(const char* resource, const char* kegeratorKey, const char* authToken)
{
	this->client.stop();

	this->client.beginRequest();
	int connectionStatus = this->client.post(uri, resource);
	if (connectionStatus < 0) 
	{
		DBG("Status:");
		DBG(connectionStatus);
		return;
	}
	this->client.sendHeader("Content-type: application/json");
	this->client.sendHeader(KegeratorCommunicationKey, kegeratorKey);
	this->client.sendHeader("Cookie", authToken);

	//this->client.write((uint8_t*)"Cookie: .ASPXAUTH=", 18);
	//this->client.write((uint8_t*)authToken, strlen(authToken));
}

void RestClient::GetJson()
{
	int code = this->client.responseStatusCode();

	// Something went crazy
	if (code < 0)
	{
		DBG("ERR1");
		return;
	}
	
	// Skip the headers -- we don't really care about this
	int headerValue = this->client.skipResponseHeaders();

	if (headerValue < 0)
	{
		return;
	}

	int contentLength = this->client.contentLength();

	// Something went horribly wrong here..
	if (contentLength <= 0)
	{
		return;
	}

	if (this->currentResponse != NULL)
	{
		this->Cleanup();
	}

	contentLength++;

	//this->currentResponse = new uint8_t[contentLength];
	memset((void*)currentResponse, '\0', 300);  // Get rid of any garbage so this will be read correctly
	int i = 0;

	this->client.read(currentResponse, contentLength);
	
	DBG((char*)currentResponse);
	DBG("\r\n\r\n");

	// do some json magic 
	int tokenOutput = json_to_token_list((char*)currentResponse, this->token_list);
	
	char m[] = "Message";
	char * out = json_get_value(this->token_list, m);

	if (out != NULL) 
	{
		this->Cleanup();
	}
}

void RestClient::Cleanup()
{
	return; 
	/*
	if (this->currentResponse == NULL)
	{
		return;
	}

	delete [] currentResponse;
	currentResponse = NULL;
	*/
	//release_token_list(this->token_list);
	//this->token_list = NULL;
}

PourInfo& RestClient::Validate(const char* kegeratorKey, const char* authToken)
{
	PourInfo info;

	this->Setup(PourResource, kegeratorKey, authToken);
	this->client.sendHeader(CONTENT_LENGTH, 0);
	this->client.endRequest();

	this->GetJson();

	if (this->token_list->count == 0)
	{
		return info;
	}

	char* temp = NULL;
	
	char availableOunces[] = "AvailableOuncesI";
	temp = json_get_value(this->token_list, availableOunces);
	if (temp != NULL)
	{
		info.AvailableOunces = atoi(temp);
	}

	char uniqueID[] = "UniqueID";
	temp = json_get_value(this->token_list, uniqueID);
	if (temp != NULL)
	{
		info.UniqueID = atoi(temp);
	}
	
	char pourKey[] = "PourKey";
	info.PourKey = json_get_value(this->token_list, pourKey);
	
	return info;
}
#define JSON1 "{\"AvailableOuncesI\":"
#define JSON2 ",\"PourKey\":\""
#define JSON3 "\",\"UniqueID\":"
#define JSON4 ",\"PouredOuncesI\":"
#define JSON5 "}"

bool RestClient::Pour(const char* kegeratorKey, const char* authToken, PourInfo& info)
{
	int contentLength = strlen(JSON1) + strlen(JSON2) + strlen(JSON3) + strlen(JSON4) + strlen(JSON5) + strlen(info.PourKey);

	char buff[10];
	
	// do this calculation... kinda sucks but best way to only use the buffer once
	itoa(info.AvailableOunces, buff, 10);
	contentLength += strlen(buff);
	itoa(info.UniqueID, buff, 10);
	contentLength += strlen(buff);
	itoa(info.PouredOunces, buff, 10);
	contentLength += strlen(buff);
	
	this->Setup(PourResource, kegeratorKey, authToken);
	this->client.sendHeader(CONTENT_LENGTH, contentLength);

	this->client.write((uint8_t*)JSON1, strlen(JSON1));
	itoa(info.AvailableOunces, buff, 10);
	this->client.write((uint8_t*)buff, strlen(buff));
	this->client.write((uint8_t*)JSON2, strlen(JSON2));
	this->client.write((uint8_t*)info.PourKey, strlen(info.PourKey));
	this->client.write((uint8_t*)JSON3, strlen(JSON3));
	itoa(info.UniqueID, buff, 10);
	this->client.write((uint8_t*)buff, strlen(buff));
	this->client.write((uint8_t*)JSON4, strlen(JSON4));
	itoa(info.PouredOunces, buff, 10);
	this->client.write((uint8_t*)buff, strlen(buff));
	this->client.write((uint8_t*)JSON5, strlen(JSON5));

	this->client.endRequest();

	this->GetJson();

	if (this->token_list == NULL)
	{
		return false;
	}

	DBG("Success");

	return true;
}