#pragma once


#ifdef SHADER_EXPORT_LIB 
#define SHADER_API_LIB __declspec(dllexport)
#else
#define SHADER_API_LIB __declspec(dllimport)
#endif