#pragma once

#ifdef MATERIAL_EXPORT_LIB 
#define MATERIAL_API_LIB __declspec(dllexport)
#else 
#define MATERIAL_API_LIB __declspec(dllimport)
#endif