#include "Debugging.h"
#include <stdio.h>
#include <cstdarg>
#include <string>

#ifdef DEBUG

void debugLog(std::string str, ...) {
	va_list args;
	va_start(args, str);
	vprintf((str.append("\n")).c_str(), args);
	va_end(args);
}

#else

void debugLog(std::string str, ...) {}

#endif