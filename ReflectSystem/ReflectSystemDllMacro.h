#pragma once



#ifdef REFLECT_SYSTEM_LIB 
#define REFLECT_SYSTEM_API __declspec(dllexport)
#else
#define REFLECT_SYSTEM_API __declspec(dllimport)
#endif