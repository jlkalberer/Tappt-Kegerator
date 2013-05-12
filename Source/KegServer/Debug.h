
#ifndef DEBUG_H__
#define DEBUG_H__

#include "MemoryFree.h"

#define DEBUG

#ifdef DEBUG
#define DBG(message)    Serial.print(message)
#define DBGB(message, base)    Serial.print(message, base)
#define DBGL(message)    Serial.println(message)
#define DBGBL(message, base)    Serial.println(message, base)
#define Memory() DBG("\r\nFree Memory: "); DBG(freeMemory());DBG("\r\n");
#else
#define DBG(message)
#define Memory()
#endif // DEBUG

#endif // __DEBUG_H__
