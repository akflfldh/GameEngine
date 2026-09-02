#pragma once

#include <CoreDevice/CoreDeviceHeader.h>
#include <vector>
namespace GRM
{

enum class EBufferUsage
{
    eDefault = 0,      // 일반 GPU 리소스 (DEFAULT)
    eVertexBuffer,     // 정점 버퍼
    eIndexBuffer,      // 인덱스 버퍼
    eConstantBuffer,   // 상수 버퍼
    eStructuredBuffer, // 구조화된 버퍼
};

enum class EBufferMemoryAccess
{
    eGpuOnly = 0, // GPU 전용(default 힙)
    eCpuWriteOnly // CPU 쓰기(upload힙)
};

// 상수버퍼: ElementDatasSIZE상수버퍼하나에들어가는데이터크기 -> 정렬
// ElementDataNum은 연속된 상수버퍼의 개수
// mData를 nullptr로 초반에 넘긴다.
// 상수버퍼의 경우에는 연속된것으로 제공하자

struct BufferDesc
{
    void *mData = nullptr;       // 데이터
    size_t mElementDataNum = 0;  // 데이터요소의수			//vertexbuffer의경우 vertex의 수
    size_t mElementDataSize = 0; // 한 데이터요소의 크기		//vertexbuffer의 경우 vertex하나의 크기

    size_t mBufferSize = 0; // buffer의최종크기

    EBufferUsage mBufferUsage = EBufferUsage::eDefault;
    EBufferMemoryAccess mBufferMemoryAccess = EBufferMemoryAccess::eGpuOnly;
};

// 밑에 3개의 enum은 DirectXTex를 기반으로만들었기에 동일하다.
//
enum ETEX_MISC_FLAG : uint32_t
// Subset here matches D3D10_RESOURCE_MISC_FLAG and D3D11_RESOURCE_MISC_FLAG
{
    TEX_MISC_TEXTURECUBE = 0x4L,
};

enum ETEX_MISC_FLAG2 : uint32_t
{
    TEX_MISC2_ALPHA_MODE_MASK = 0x7L,
};

enum ETEX_ALPHA_MODE : uint32_t
// Matches DDS_ALPHA_MODE, encoded in MISC_FLAGS2
{
    TEX_ALPHA_MODE_UNKNOWN = 0,
    TEX_ALPHA_MODE_STRAIGHT = 1,
    TEX_ALPHA_MODE_PREMULTIPLIED = 2,
    TEX_ALPHA_MODE_OPAQUE = 3,
    TEX_ALPHA_MODE_CUSTOM = 4,
};

enum class ESamplerFilterMode
{
    eMIN_MAG_MIP_LINEAR = 0,
    eMIN_MAG_MIP_POINT
};

enum class ESamplerAddressMode
{
    eWrap = 0,
    eClamp
};

struct SamplerDesc
{
    // filter
    ESamplerFilterMode mFilterMode;

    // address mode
    ESamplerAddressMode mAddressU;
    ESamplerAddressMode mAddressV;
    ESamplerAddressMode mAddressW;
};

} // namespace GRM