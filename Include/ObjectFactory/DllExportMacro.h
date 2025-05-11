#pragma once


#ifdef OBJECTFACTORY_LIB_EXPORT
#define OBJECTFACTORY_API_LIB __declspec(dllexport)
#else
#define OBJECTFACTORY_API_LIB __declspec(dllimport)
#endif