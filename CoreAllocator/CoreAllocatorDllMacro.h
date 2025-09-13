#pragma once

#ifdef CORE_ALLOC_LIB
#define CORE_ALLOC_API __declspec(dllexport)
#else
#define CORE_ALLOC_API __declspec(dllimport)
#endif
