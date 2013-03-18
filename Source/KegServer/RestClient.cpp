#include "RestClient.h"

using namespace KegServer;

RestClient::RestClient(WiflyClass *wifly, const char *restUrl)
{
	this->port = "80"; // This could change when we use SSL
	this->restUrl = restUrl;

	this->wifly = wifly;
}

bool RestClient::Request(RestRequest &request)
{
	wifly->sendCommand("$$$", "CMD");
	wifly->sendCommand("set ip proto 18\r", AOK); // set as HTTP request
	wifly->sendCommand("set dns name ", "");  // multi-line setup for the host
	wifly->sendCommand(this->restUrl, "");
	wifly->sendCommand("\r", AOK);
	wifly->sendCommand("set ip address 0\r", AOK); // turn on DNS
	wifly->sendCommand("set ip remote 80\r", AOK); // set the web-server port

	// execute
	wifly->sendCommand("open ", "");
	wifly->sendCommand(this->restUrl, "", true);
	wifly->sendCommand(" ", "", true);
	wifly->sendCommand(this->port, "", true);
	boolean ret = wifly->sendCommand("\r", "*OPEN*", true);
	if(!ret) {
		wifly->sendCommand("exit\r", "EXIT");
		return false;
	}
	return true;
}
