#pragma once
#include <stdint.h>

// vector나 배열에 담겨서도 통쨰로 안전하게 직렬화가될수있는 원소 타입 식별 태그 역할
// 향후 엔디안 문제도 다루어야한다.
template <typename T> struct Blitable_Trait
{
    static constexpr bool value = false;
};

template <> struct Blitable_Trait<uint32_t>
{
    static constexpr bool value = true;
};
template <> struct Blitable_Trait<uint8_t>
{
    static constexpr bool value = true;
};
template <> struct Blitable_Trait<uint16_t>
{
    static constexpr bool value = true;
};

template <> struct Blitable_Trait<uint64_t>
{
    static constexpr bool value = true;
};

template <> struct Blitable_Trait<int8_t>
{
    static constexpr bool value = true;
};
template <> struct Blitable_Trait<int16_t>
{
    static constexpr bool value = true;
};
template <> struct Blitable_Trait<int32_t>
{
    static constexpr bool value = true;
};

template <> struct Blitable_Trait<int64_t>
{
    static constexpr bool value = true;
};
template <> struct Blitable_Trait<float>
{
    static constexpr bool value = true;
};

template <> struct Blitable_Trait<double>
{
    static constexpr bool value = true;
};

template <> struct Blitable_Trait<bool>
{
    static constexpr bool value = true;
};
