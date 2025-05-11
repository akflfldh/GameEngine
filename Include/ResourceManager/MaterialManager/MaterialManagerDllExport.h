#pragma once

#ifdef MATERIALMANAGER_EXPORT_LIB 
#define MATERIALMANAGER_API_LIB __declspec(dllexport)
#else
#define MATERIALMANAGER_API_LIB __declspec(dllimport)
#endif