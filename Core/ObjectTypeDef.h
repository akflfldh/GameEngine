#pragma once

#include <stdint.h>

namespace Core
{

struct ObjectHandle
{
    uint64_t mObjectPoolIndex;
    uint64_t mGeneration;
};

} // namespace Core