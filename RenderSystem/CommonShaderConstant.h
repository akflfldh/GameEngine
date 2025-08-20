#pragma once

#ifdef __HLSL__
#define CBUFFER_BEGIN(name, reg)                                                                                       \
    cbuffer name : register(reg)                                                                                       \
    {
#define CBUFFER_END }

using Matrix4X4 = float4x4;
using Vector4 = float4;
using Vector3 = float3;
using Vector2 = float2;

#else
#define CBUFFER_BEGIN(name, reg)                                                                                       \
    struct name                                                                                                        \
    {
#define CBUFFER_END                                                                                                    \
    }                                                                                                                  \
    ;

#include "CoreMath.h"
using namespace CoreMath;

#endif

namespace EngineShaderConstants
{
CBUFFER_BEGIN(PassConstants, b0) // b0 레지스터에 바인딩
Matrix4X4 mViewProjMatrix;

CBUFFER_END

CBUFFER_BEGIN(ObjectConstants, b1) // b1 레지스터에 바인딩
Matrix4X4 mWorldMatrix;

CBUFFER_END

} // namespace EngineShaderConstants
