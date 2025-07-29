#pragma once


#ifdef GPURESOURCE_MANAGER_LIB 
#define GPURESOURCE_MANAGER_API __declspec(dllexport)
#else
#define GPURESOURCE_MANAGER_API __declspec(dllimport)
#endif