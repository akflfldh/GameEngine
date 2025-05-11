#pragma once



#ifdef TEXTURE_EXPORT_LIB 
#define TEXTURE_API_LIB __declspec(dllexport)
#else
#define TEXTURE_API_LIB __declspec(dllimport)
#endif