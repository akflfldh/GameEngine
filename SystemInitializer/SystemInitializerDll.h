#pragma once



#ifdef SYSTEM_INITIALIZER_LIB 
#define SYSTEM_INITIALIZER_API __declspec(dllexport)
#else
#define SYSTEM_INITIALIZER_API __declspec(dllimport)
#endif