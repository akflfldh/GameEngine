#pragma once


#ifdef TEXTUREMANAGER_EXPORT_LIB
#define TEXTUREMANAGER_API_LIB __declspec(dllexport)
#else
#define TEXTUREMANAGER_API_LIB __declspec(dllimport)
#endif
