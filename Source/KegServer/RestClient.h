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

namespace KegServer
{
	// A basic client for making rest requests.
	class RestClient
	{
	public:
		// The constructor which takes in a wifly instance and the endpoint of the service.
		RestClient(WiflyClass *wifly, const char *restUrl);

		// Make a request
		bool Request(RestRequest &request);
	private:
		// The port to send the request through.
		const char* port;
		// The endpoint of the rest call.
		const char* restUrl;
		// The wifly instance for making the request through the hardware.
		WiflyClass* wifly;
	};
}
#endif