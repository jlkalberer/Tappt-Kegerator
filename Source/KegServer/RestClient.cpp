#include "RestClient.h"
#include "Debug.h"

#define Pour "/api/pour"

// The kegerator communication key.
#define KegeratorCommunicationKey "Kegerator-Communication-Key"

/// The forms authentication cookie.
#define FormsAuthenticationCookie ".ASPXAUTH"

RestClient::RestClient(WiFly& wifly, const char* uri)
	: client(wifly)
{
	this->uri = uri;
	this->currentResponse = NULL;
	this->token_list = NULL;
}

void RestClient::Setup(const char* resource, const char* kegeratorKey, const char* authToken)
{
	char cookie[300];
	sprintf(cookie, "Cookie: %s=%s", FormsAuthenticationCookie, authToken);

	this->client.beginRequest();
	int connectionStatus = this->client.post(uri, resource);
	if (connectionStatus < 0) 
	{
		DBG("Status:");
		DBG(connectionStatus);
		return;
	}
	this->client.sendHeader(KegeratorCommunicationKey, kegeratorKey);
	this->client.sendHeader(cookie);
}

void RestClient::GetJson()
{
	int code = this->client.responseStatusCode();

	// Something went crazy
	if (code < 0)
	{
		DBG("Response Code Error:");
		DBG(code);
		return;
	}
	
	// Skip the headers -- we don't really care about this
	int headerValue = this->client.skipResponseHeaders();

	if (headerValue < 0)
	{
		DBG("Header Value Error:");
		DBG(headerValue);
		return;
	}

	int contentLength = this->client.contentLength();

	// Something went horribly wrong here..
	if (contentLength <= 0)
	{
		DBG("Content Length Error:");
		DBG(contentLength);
		return;
	}

	if (this->currentResponse != NULL)
	{
		delete [] currentResponse;
		release_token_list(this->token_list);
	}

	this->currentResponse = new uint8_t[contentLength + 1];
	memset((void*)currentResponse, 0, sizeof(uint8_t) * contentLength);  // Get rid of any garbage so this will be read correctly
	currentResponse[contentLength] = '\0';
	int i = 0;
	
	this->client.read(currentResponse, sizeof(uint8_t) * contentLength);

	delay(500);
	// do some json magic 

	DBG((char*)currentResponse);
	
	token_list = create_token_list(30);
	json_to_token_list((char*)currentResponse, token_list);

	Serial.print("Found Pairs: ");
	Serial.println(token_list->count);
	char * out = json_get_value(token_list, "Message");
	Serial.print("BLEH");
	Serial.println(out);
}

PourInfo& RestClient::Validate(const char* kegeratorKey, const char* authToken)
{
	PourInfo info;

	this->Setup(Pour, kegeratorKey, authToken);
	this->client.sendHeader("Content-Length", 0);
	this->client.endRequest();

	this->GetJson();
}