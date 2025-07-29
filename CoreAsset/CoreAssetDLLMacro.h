#pragma once

#pragma warning(disable: 4251)

#ifdef CORE_ASSET_EXPORT
#define CORE_ASSET_API __declspec(dllexport)
#else
#define CORE_ASSET_API __declspec(dllimport)
#endif
