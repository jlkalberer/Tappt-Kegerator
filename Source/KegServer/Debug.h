
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "MemoryFree.h"

#define DEBUG

#ifdef DEBUG
#define DBG(message)    Serial.print(message)
#define Memory() DBG("\r\nFree Memory: "); DBG(freeMemory());DBG("\r\n");
#else
#define DBG(message)
#endif // DEBUG

#endif // __DEBUG_H__
