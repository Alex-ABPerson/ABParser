#pragma once

#define COMPILE_DLL

#ifdef COMPILE_DLL

#define _CRT_SECURE_NO_WARNINGS
#define EXPORT __declspec(dllexport)

#else

#define EXPORT

#endif