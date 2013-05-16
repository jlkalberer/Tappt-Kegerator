#ifndef REST_CLIENT_H
#define REST_CLIENT_H

#include "HTTPClient.h"
#include "PourInfo.h"

class RestClient 
{
public:
	/**
	*	Initialize the rest client.
	**/
	RestClient(WiFly& wifly, const char* uri);
	/**
	*	Used to validate that the user can actually perform the pour.
	**/
	PourInfo* Validate(const char* kegeratorKey, const char* authToken);
	
	/**
	*	Used to send the pour to the server.
	**/
	bool Pour(const char* kegeratorKey, const char* authToken, PourInfo* info);
	
	/**
	* DCTOR
	**/
	~RestClient();
private:
	/**
	*	Setup the initial request by setting common values.
	**/
	void Setup(const char* resource, const char* kegeratorKey, const char* authToken);

	void GetResponse();

	void Cleanup();

	HttpClient client;
	const char* uri;
	uint8_t *currentResponse;
	int contentLength;
	PourInfo *pourInfo;
};

#endif