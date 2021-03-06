//Board = Arduino Uno
#define ARDUINO 105
#define __AVR_ATmega328P__
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
#define __attribute__(x)
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__
#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define prog_void
#define PGM_VOID_P int
#define NOINLINE __attribute__((noinline))

typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {}

//already defined in arduno.h
//already defined in arduno.h
void ReadTemperature();
void flowCounter();

#include "C:\Dev\arduino-1.0.4\hardware\arduino\variants\standard\pins_arduino.h" 
#include "C:\Dev\arduino-1.0.4\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\KegServer.ino"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\Debug.h"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\HTTPClient.cpp"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\HTTPClient.h"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\MemoryFree.cpp"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\MemoryFree.h"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\NFC.cpp"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\NFC.h"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\PourInfo.h"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\RestClient.cpp"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\RestClient.h"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\Temperature.cpp"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\Temperature.h"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\b64.cpp"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\b64.h"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\jsmn.cpp"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\jsmn.h"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\json_arduino.cpp"
#include "C:\Dev\Mine\Tappt-KegServer\Source\KegServer\json_arduino.h"
