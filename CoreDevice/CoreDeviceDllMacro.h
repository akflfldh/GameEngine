#pragma once


#ifdef CORE_DEVICE_LIB 
#define CORE_DEVICE_API __declspec(dllexport)
#else
#define CORE_DEVICE_API __declspec(dllimport)
#endif