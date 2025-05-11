#pragma once


#ifdef ALLOCATOR_EXPORT_LIB
#define ALLOCATOR_API_LIB __declspec(dllexport)
#else 
#define ALLOCATOR_API_LIB __declspec(dllimport)
#endif
