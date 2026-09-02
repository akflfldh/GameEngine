#pragma once
#include <CoreBase/CoreBaseDllMacro.h>

bool COREBASE_API ReportAssertFailure(const char *condition, const char *file, int line, const char *msg, ...);
bool COREBASE_API ReportAssertFailure(const char *condition, const char *file, int line);

#ifdef _DEBUG

#define CHECK(condition, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(condition))                                                                                              \
        {                                                                                                              \
            ReportAssertFailure(#condition, __FILE__, __LINE__ __VA_OPT__(, ) __VA_ARGS__);                            \
            __debugbreak();                                                                                            \
        }                                                                                                              \
    } while (0)

#else

#define CHECK(condition) (void(0))

#endif
