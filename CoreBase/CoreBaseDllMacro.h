

#ifdef COREBASE_EXPORT_LIB
#define COREBASE_API __declspec(dllexport)
#else
#define COREBASE_API __declspec(dllimport)
#endif