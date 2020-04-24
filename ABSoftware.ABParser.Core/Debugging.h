#pragma once
#include <string>

// Enable to show all of the logging, in order to help identify where problems are occuring.
#define DEBUG

#ifdef DEBUG
void debugLog(std::string str, ...);
#else
void debugLog(std::string str, ...);
#endif