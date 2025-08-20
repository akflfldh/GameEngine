#pragma once


#ifdef QUAD_CALLBACK_SYSTEM_LIB
#define QUAD_CALLBACK_SYSTEM_API __declspec(dllexport)
#else 
#define QUAD_CALLBACK_SYSTEM_API __declspec(dllimport)
#endif