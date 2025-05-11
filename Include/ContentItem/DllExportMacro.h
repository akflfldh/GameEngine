#pragma once



#ifdef CONTENTITEM_EXPORT_LIB 
#define CONTENTITEM_API_LIB __declspec(dllexport)
#else
#define CONTENTITEM_API_LIB __declspec(dllimport)
#endif
