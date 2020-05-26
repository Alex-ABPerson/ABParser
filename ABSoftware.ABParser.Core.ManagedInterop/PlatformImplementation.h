//#pragma once

#define COMPILE_DLL

#ifdef COMPILE_DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif