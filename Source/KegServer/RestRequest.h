#ifndef RESTREQUEST_H
#define RESTREQUEST_H

#include "vector.h"
#include "Parameter.h"
#include "Method.h"

namespace KegServer
{
	// Encapsulation of a REST request.
	class RestRequest
	{
	public:
		// Constructor for accessing a resource
		RestRequest(const char* resource);

		// Adds a cookie to the request.
		void AddCookie(const char* cookieName, const char* value);

		// Adds a value to the request header.
		void AddHeader(const char* headerName, const char* value);
		
		// Adds a parameter to the request.
		void AddParameter(const char* parameterName, const char* value);

		// The method to access the resource with.
		KegServer::Method Method;
	private:
		// The resource of the request.
		const char* resource;
		// The collection of headers.
		Vector<Parameter> headers;
		// The collection of cookies.
		Vector<Parameter> cookies;
		// The collection of parameters.
		Vector<Parameter> parameters;
	};
}
#endif