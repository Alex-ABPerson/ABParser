#pragma once
#include <string>
#include <stdio.h>
#include <cstdarg>

// Enable to show all of the logging, in order to help identify where problems are occuring.
//#define DEBUG

#ifdef DEBUG
inline void debugLog(std::string str, ...) {
	va_list args;
	va_start(args, str);
	vprintf((str.append("\n")).c_str(), args);
	va_end(args);
}
#else
inline void debugLog(std::string str, ...) {}
#endif