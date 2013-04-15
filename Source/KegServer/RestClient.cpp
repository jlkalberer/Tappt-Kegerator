#include "RestClient.h"
#include "Debug.h"
#include "MemoryFree.h"


#define Pour "/api/pour"

// The kegerator communication key.
#define KegeratorCommunicationKey "Kegerator-Communication-Key"

RestClient::RestClient(WiFly& wifly, const char* uri)
	: client(wifly)
{
	this->uri = uri;
	this->currentResponse = NULL;
	this->token_list = NULL;
}

void RestClient::Setup(const char* resource, const char* kegeratorKey, const char* authToken)
{
	this->client.beginRequest();
	int connectionStatus = this->client.post(uri, resource);
	if (connectionStatus < 0) 
	{
		DBG("Status:");
		DBG(connectionStatus);
		return;
	}
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
		delete [] currentResponse;
		release_token_list(this->token_list);
	}

	contentLength++;

	this->currentResponse = new uint8_t[contentLength];
	memset((void*)currentResponse, '\0', contentLength);  // Get rid of any garbage so this will be read correctly
	int i = 0;

	this->client.read(currentResponse, contentLength);
	
	DBG((char*)currentResponse);

	// do some json magic 
	token_list = create_token_list(9);
	int tokenOutput = json_to_token_list((char*)currentResponse, token_list);
	
	char m[] = "Message";
	char * out = json_get_value(token_list, m);

	if (out != NULL) 
	{
		delete [] currentResponse;
		currentResponse = NULL;
		release_token_list(this->token_list);

		this->token_list = NULL;
	}
}

PourInfo& RestClient::Validate(const char* kegeratorKey, const char* authToken)
{
	PourInfo info;

	this->Setup(Pour, kegeratorKey, authToken);
	this->client.sendHeader("Content-Length", 0);
	this->client.endRequest();

	this->GetJson();

	if (this->token_list == NULL)
	{
		return info;
	}

	char* temp = NULL;
	
	temp = json_get_value(this->token_list, "AvailableOunces");
	if (temp != NULL)
	{
		info.AvailableOunces = atof(temp);
	}

	info.PourKey = json_get_value(this->token_list, "PourKey");
	DBG(info.PourKey);

	temp = json_get_value(this->token_list, "UniqueID");
	if (temp != NULL)
	{
		info.UniqueID = atoi(temp);
	}

	return info;
}