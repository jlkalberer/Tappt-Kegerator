#include "RestClient.h"
#include "Debug.h"
#include "MemoryFree.h"


#define PourResource "/api/pour"

// The kegerator communication key.
#define KegeratorCommunicationKey "TapptAuth"
#define CONTENT_TYPE "Content-type: text/plain"

RestClient::RestClient(WiFly& wifly, const char* uri)
	: client(wifly)
{
	this->uri = uri;
	this->currentResponse = NULL;
	this->pourInfo = new PourInfo();
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
	this->client.sendHeader(CONTENT_TYPE);
	this->client.sendHeader(KegeratorCommunicationKey, kegeratorKey);
	this->client.sendHeader("TapptKegAuth", authToken);
}

void RestClient::GetResponse()
{
	int code = this->client.responseStatusCode();
	this->pourInfo->PourKey = NULL;
	this->contentLength = 0;

	// Something went crazy
	if (code < 0)
	{
		this->Cleanup();
		DBGL("ERR1");
		return;
	}
	
	if (code > 299)
	{
		this->Cleanup();
		DBGL("ERR_RESPONSE");
		return;
	}

	// Skip the headers -- we don't really care about this
	int headerValue = this->client.skipResponseHeaders();

	if (headerValue < 0)
	{
		DBGL("Header?");
		return;
	}

	this->contentLength = this->client.contentLength();

	// Something went horribly wrong here..
	if (contentLength <= 0)
	{
		DBGL("No content length");
		return;
	}

	if (this->currentResponse != NULL)
	{
		this->Cleanup();
	}

	contentLength++;
	this->currentResponse = new uint8_t[this->contentLength];
	memset((void*)currentResponse, '\0', this->contentLength);  // Get rid of any garbage so this will be read correctly
	int i = 0;

	this->client.read(currentResponse, this->contentLength);

	this->pourInfo->PourKey = (char*)this->currentResponse;
}

PourInfo* RestClient::Validate(const char* kegeratorKey, const char* authToken)
{
	//this->pourInfo = NULL;
	this->Setup(PourResource, kegeratorKey, authToken);
	this->client.write(HttpClient::kContentLengthPrefix);
	this->client.write(0);
	this->client.writeln();
	this->client.endRequest();
	
	this->GetResponse();
	
	if (this->pourInfo->PourKey == NULL) 
	{
		return NULL;
	}

	return this->pourInfo;
}
bool RestClient::Pour(const char* kegeratorKey, PourInfo* info)
{
	this->Setup(PourResource, kegeratorKey, (const char*)this->currentResponse);

	char buffer[10];
	memset((void*)buffer, '\0', 10);
	itoa(info->PouredOunces, buffer, 10);
	int length = strlen(buffer) + this->contentLength + 1; 

	this->client.write(HttpClient::kContentLengthPrefix);
	this->client.write(length);
	this->client.writeln();

	this->client.write((uint8_t*)buffer, strlen(buffer));
	this->client.write((uint8_t*)":", 1);
	this->client.write(this->currentResponse, this->contentLength);
	
	this->client.endRequest();

	this->Cleanup();
	
	return true;
}


void RestClient::Cleanup()
{
	if (this->currentResponse == NULL)
	{
		return;
	}

	delete [] currentResponse;
	currentResponse = NULL;
}


RestClient::~RestClient()
{
	if (this->currentResponse != NULL)
	{
		delete [] currentResponse;
		currentResponse = NULL;
	}
}