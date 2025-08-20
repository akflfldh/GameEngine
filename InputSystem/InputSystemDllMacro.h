


#ifdef INPUT_SYSTEM_LIB 
#define INPUT_SYSTEM_API __declspec(dllexport)
#else
#define INPUT_SYSTEM_API __declspec(dllimport)
#endif