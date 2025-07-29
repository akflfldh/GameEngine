#pragma once


#ifdef PHYSICAL_FILESYSTEM_EXPORT
#define PHYSICAL_FILESYSTEM_API __declspec(dllexport)
#else
#define PHYSICAL_FILESYSTEM_API __declspec(dllimport)
#endif