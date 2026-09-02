#pragma once

#include <stdint.h>

constexpr uint32_t HashString(const char *str, size_t length)
{
    uint32_t hash = 2166136261u;
    uint32_t prime = 16777619u;

    for (size_t i = 0; i < length; ++i)
    {
        hash = hash ^ (uint32_t)str[i];
        hash *= prime;
    }

    return hash;
}

consteval uint32_t operator""_sid(const char *str, size_t len)
{
    return HashString(str, len);
}