// (c) Copyright 2010-2012 MCQN Ltd.
// Released under Apache License, version 2.0
//
// Simple example to show how to use the HttpClient library
// Get's the web page given at http://<kHostname><kPath> and
// outputs the content to the serial port

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <WiFly.h>
#include "HttpClient.h"
#include "json_arduino.h"

#define SSID      "casanova"
#define PASSWORD       "bitchhunter"
#define AUTH      WIFLY_AUTH_WPA2_PSK

#define HTTP_GET_URL "httpbin.org"
#define HTTP_GET_PATH "/user-agent"
#define HTTP_POST_URL "http://httpbin.org/post"
#define HTTP_POST_DATA "Hello world!"

// Name of the server we want to connect to
const char kHostname[] = "httpbin.org";
// Path to download (this is the bit after the hostname in the URL
// that you want to download
const char kPath[] = "/user-agent";

#define LOGGING 1

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

WiFly wifly(2, 3);

void setup()
{
	// initialize serial communications at 9600 bps:
	Serial.begin(9600); 

	wifly.reset();

	while (1) {
		Serial.println("Try to join " SSID );
		if (wifly.join(SSID, PASSWORD, AUTH)) {
			Serial.println("Succeed to join " SSID);
			wifly.clear();
			break;
		} else {
			Serial.println("Failed to join " SSID);
			Serial.println("Wait 1 second and try again...");
			delay(1000);
		}
	}

	delay(3000);
}

void loop()
{
	int err =0;

	HttpClient http(wifly);

	err = http.get(kHostname, kPath);

	if (err == 0)
	{
		Serial.println("startedRequest ok");

		err = http.responseStatusCode();
		if (err >= 0)
		{
			Serial.print("Got status code: ");
			Serial.println(err);

			// Usually you'd check that the response code is 200 or a
			// similar "success" code (200-299) before carrying on,
			// but we'll print out whatever response we get
			/*char get;
			while (wifly.receive((uint8_t *)&get, 1, 1000) == 1) {
				Serial.print(get);
			}
			while(1);*/

			err = http.skipResponseHeaders();
			if (err >= 0)
			{
				int bodyLen = http.contentLength();
				Serial.print("Content length is: ");
				Serial.println(bodyLen);
				Serial.println();

				// Now we've got to the body, so we can print it out
				unsigned long timeoutStart = millis();
				char c;

				//uint8_t *body = new uint8_t[bodyLen];
				uint8_t *body = new uint8_t[bodyLen + 1];
				memset((void*)body, 0, sizeof(uint8_t) * bodyLen);  // Get rid of any garbage so this will be read correctly
				body[bodyLen] = '\0';
				int i = 0;
				
				Serial.println("Body returned follows:");

				http.read(body, sizeof(uint8_t) * bodyLen);
				// Whilst we haven't timed out & haven't reached the end of the body
				//while ( http.connected() &&
				//	((millis() - timeoutStart) < kNetworkTimeout) )
				//{
				//	c = http.read();
				//	
				//	if (c >= 0)
				//	{
				//		// Print out this character
				//		body[i] = c;

				//		bodyLen--;
				//		i++;
				//		// We read something, reset the timeout counter
				//		timeoutStart = millis();
				//	}
				//	//else
				//	//{
				//	//	// We haven't got any data, so let's pause to allow some to
				//	//	// arrive
				//	//	delay(kNetworkDelay);
				//	//}
				//}

				Serial.write(body, bodyLen);

				token_list_t *token_list = create_token_list(30);
				json_to_token_list((char*)body, token_list);

				Serial.print("Found Pairs: ");
				Serial.println(token_list->count);
				
				char * out = json_get_value(token_list, "user-agent");
				Serial.println("FOOBAR");
				Serial.println(out);
				
			}
			else
			{
				Serial.print("Failed to skip response headers: ");
				Serial.println(err);
			}
		}
		else
		{    
			Serial.print("Getting response failed: ");
			Serial.println(err);
		}
	}
	else
	{
		Serial.print("Connect failed: ");
		Serial.println(err);
	}
	http.stop();

	Serial.print("\rComplete\r");

	// And just stop, now that we've tried a download
	while(1);
}

char* GetValue(jsmntok_t &token, uint8_t * buffer)
{
	char * output = new char[token.size + 1];
	strncpy(output, (char*)buffer + token.start, token.size);
	output[token.size] = 0;
	return output;
}
