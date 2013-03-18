#include "RestRequest.h"

using namespace KegServer;

RestRequest::RestRequest(const char* resource)
{
	// default to get request
	this->Method = GET;
	this->resource = resource;
}

void RestRequest::AddCookie(const char* cookieName, const char* value)
{
	this->cookies.push_back(Parameter(cookieName, value));
}

void RestRequest::AddHeader(const char* headerName, const char* value)
{
	this->headers.push_back(Parameter(headerName, value));
}

void RestRequest::AddParameter(const char* parameterName, const char* value)
{
	this->parameters.push_back(Parameter(parameterName, value));
}