

#ifdef RENDER_FRONTEND_LIB
#define RENDER_FRONTEND_API __declspec(dllexport)
#else
#define RENDER_FRONTEND_API __declspec(dllimport)
#endif