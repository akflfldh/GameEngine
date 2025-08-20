#pragma once


#ifdef CORE_EXPORT_LIB 
#define CORE_API_LIB __declspec(dllexport)
#else
#define CORE_API_LIB __declspec(dllimport)
#endif