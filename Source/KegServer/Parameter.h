#ifndef PARAMETER_H
#define PARAMETER_H

namespace KegServer
{
	class Parameter
	{
	public:
		// Constructor with key/value pair.
		Parameter(const char* key, const char* value);
		// The key of the parameter.
		const char* Key;
		// The value of the parameter.
		const char* Value;
	};
}

#endif