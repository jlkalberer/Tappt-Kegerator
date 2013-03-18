#include "Parameter.h"

using namespace KegServer;

Parameter::Parameter(const char* key, const char* value)
{
	this->Key = key;
	this->Value = value;
}