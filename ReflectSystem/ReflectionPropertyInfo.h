#pragma once
#include "ReflectSystem/ReflectSystemDllMacro.h"

namespace Quad
{
struct REFLECT_SYSTEM_API PropertyInfo
{
    PropertyInfo(const char *propertyname, const char *t, size_t offset)
    {
        mPropertyName = propertyname;
        mType = t;
        mOffset = offset;
    }

    const char *mPropertyName;
    const char *mType;
    size_t mOffset;
};

} // namespace Quad