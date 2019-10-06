#pragma once

#define COMPILE_DLL

#ifdef COMPILE_DLL

// Since we typically compile DLL files with the Visual C++ compiler, we need to allow use of "unsafe" methods.
#define _CRT_SECURE_NO_WARNINGS
#define EXPORT __declspec(dllexport)

#else

#define EXPORT

#endif