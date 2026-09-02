#pragma once
#include <stdint.h>

namespace UI
{

struct UIElementHandle
{
    uint64_t mGeneration;
    uint64_t mPoolSlotIndex;
};

struct UIComponentHandle
{
    uint64_t mGeneration;
    uint64_t mPoolSlotIndex;
};

enum class ETransformChangeType
{
    ePosition = 0,
    eSize,
    eScale,
    ePivot,
    eAll
};

} // namespace UI