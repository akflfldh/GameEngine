#pragma once

#ifdef COREMATH_LIB
#define COREMATH_API __declspec(dllexport)
#else
#define COREMATH_API __declspec(dllimport)
#endif
