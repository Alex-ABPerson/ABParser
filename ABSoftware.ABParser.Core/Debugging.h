#pragma once
#ifndef _ABPARSER_INCLUDE_DEBUGGING_H
#define _ABPARSER_INCLUDE_DEBUGGING_H
#include <string>

// Enable to show all of the logging, in order to help identify where problems are occuring.
//#define _ABP_IS_DEBUG

#ifdef _ABP_IS_DEBUG

#include <stdio.h>
#include <cstdarg>

void abpDebugLog(std::string str, ...) {
	va_list args;
	va_start(args, str);
	vprintf((str.append("\n")).c_str(), args);
	va_end(args);
}

#define _ABP_DEBUG_OUT(...) abpDebugLog(__VA_ARGS__);

#else
#define _ABP_DEBUG_OUT
#endif
#endif