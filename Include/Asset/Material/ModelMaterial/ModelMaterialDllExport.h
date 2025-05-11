#pragma once


#ifdef MODELMATERIAL_EXPORT_LIB 
#define MODELMATERIAL_API_LIB __declspec(dllexport)
#else
#define MDOELMATERIAL_API_LIB __declspec(dllimport)
#endif