#pragma once


#ifdef ASSET_EXPORT_LIB 
#define ASSET_API_LIB __declspec(dllexport)
#else 
#define ASSET_API_LIB __declspec(dllexport)
#endif