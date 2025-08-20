

#ifdef RENDER_SYSTEM_LIB
#define RENDER_SYSTEM_API __declspec(dllexport)
#else
#define RENDER_SYSTEM_API __declspec(dllimport)
#endif