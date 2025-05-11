#pragma once


#ifdef EFFECTMANAGER_EXPORT_LIB 
#define EFFECTMANAGER_API_LIB __declspec(dllexport)
#else
#define EFFECTMANAGER_API_LIB __declspec(dllimport)
#endif

