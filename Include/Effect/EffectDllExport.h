#pragma once


#ifdef EFFECT_EXPORT_LIB 
#define EFFECT_API_LIB __declspec(dllexport)
#else
#define EFFECT_API_LIB __declspec(dllimport)
#endif
