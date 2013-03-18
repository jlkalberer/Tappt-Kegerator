#ifndef METHOD_H
#define METHOD_H

namespace KegServer 
{
	// The method of the request.
	enum Method
	{
		GET,
		POST,
		PUT,
		DELETE,
		HEAD,
		OPTIONS,
		PATCH
	};
}

#endif