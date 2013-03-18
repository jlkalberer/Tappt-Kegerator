#ifndef RESTCLIENT_H
#define RESTCLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include "Wifly.h"
#include <SoftwareSerial.h>

#include "RestRequest.h"

#ifndef AOK
#define AOK "AOK"
#endif

// Used to access the Tappd REST service
class RestClient
{
public:
	//
	RestClient(WiflyClass *wifly, const char *restUrl);
	bool Request(RestRequest &request);
private:
	const char* port;
	const char* restUrl;
	WiflyClass* wifly;
};

#endif