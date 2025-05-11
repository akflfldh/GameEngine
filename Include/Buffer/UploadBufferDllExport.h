#pragma once


#ifdef UPLOADBUFFER_EXPORT_LIB
#define UPLOADBUFFER_API_LIB __declspec(dllexport)
#else
#define UPLOADBUFFER_API_LIB __declspec(dllimport)
#endif




