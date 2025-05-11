#pragma once


#ifdef TEXTUREFACTORY_EXPORT_LIB
#define TEXTUREFACTORY_API_LIB __declspec(dllexport)
#else
#define TEXTUREFACTORY_API_LIB __declspec(dllimport)
#endif

