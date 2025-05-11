#pragma once


#ifdef RESOURCESMARTPOINTER_EXPORT_LIB
#define RESOURCESMARTPOINTER_API_LIB __declspec(dllexport)
#else
#define RESOURCESMARTPOINTER_API_LIB __declspec(dllimport)
#endif