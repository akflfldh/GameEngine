#pragma once

#ifdef UISYSTEM_LIB
#define UISYSTEM_API __declspec(dllexport)
#else
#define UISYSTEM_API __declspec(dllimport)
#endif