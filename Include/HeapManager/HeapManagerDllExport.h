#pragma once


#ifdef HEAPMANAGER_EXPORT_LIB 
#define HEAPMANAGER_API_LIB __declspec(dllexport)
#else
#define HEAPMANAGER_API_LIB __declspec(dllimport)
#endif