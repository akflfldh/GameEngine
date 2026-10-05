#pragma once

#ifdef NETWORK_EXPORT_LIB
#define NETWORK_API __declspec(dllexport)
#else
#define NETWORK_API __declspec(dllimport)
#endif
