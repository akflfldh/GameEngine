#pragma once

#include <stdint.h>
#include <vector>
namespace GRM
{
struct DepthStecnil
{
    float mOptimizedClearDepth = 1.0f;
    unsigned char mOptimizedClearStencil = 0;
};

enum class ETextureType : uint8_t
{

    eTexture1D = 0,
    eTexture2D,
    eTexture3D
};

// 이 값들은 특정 API에 종속되지 않습니다.
enum class ETextureFormat : uint8_t
{
    eUnknown = 0, // 알 수 없는 포맷

    // --- 8-bit Formats ---
    eR8_UNORM,       // 단일 채널, 8비트, 부호 없는 정규화 (0-1)
    eR8G8_UNORM,     // 두 채널, 8비트, 부호 없는 정규화
    eR8G8B8A8_UNORM, // RGBA, 8비트, 부호 없는 정규화 (가장 일반적인 컬러 포맷)
    eR8G8B8A8_SNORM, // RGBA, 8비트, 부호 있는 정규화 (-1~1)
    eR8G8B8A8_UINT,  // RGBA, 8비트, 부호 없는 정수 (0~255)
    eR8G8B8A8_SINT,  // RGBA, 8비트, 부호 있는 정수 (-128~127)
    eB8G8R8A8_UNORM,
    eR8G8B8A8_UNORM_SRGB,
    eB8G8R8A8_UNORM_SRGB,
    eB8G8R8X8_UNORM,
    eB8G8R8X8_UNORM_SRGB,

    // --- 16-bit Formats ---
    eR16_FLOAT,          // 단일 채널, 16비트, 부동소수점 (Half-precision float)
    eR16G16_FLOAT,       // 두 채널, 16비트, 부동소수점
    eR16G16B16A16_FLOAT, // RGBA, 16비트, 부동소수점 (Half-precision float)
    eR16_UNORM,          // 단일 채널, 16비트, 부호 없는 정규화
    eR16G16_UNORM,
    eR16G16B16A16_UNORM,

    // --- 32-bit Formats ---
    eR32_FLOAT, // 단일 채널, 32비트, 부동소수점 (Full-precision float)
    eR32G32_FLOAT,
    eR32G32B32_FLOAT, // 3채널, 32비트 부동소수점 (DirectX에서는 잘 지원 안됨)
    eR32G32B32A32_FLOAT,
    eR32_UINT,
    eR32G32_UINT,
    eR32G32B32_UINT,
    eR32G32B32A32_UINT,

    // --- Depth/Stencil Formats ---
    eD32_FLOAT,         // 32비트 깊이 (float)
    eD24_UNORM_S8_UINT, // 24비트 깊이 (정규화), 8비트 스텐실 (정수)
    eD16_UNORM,         // 16비트 깊이 (정규화)

    // --- Compressed Formats (BC/DXT) ---
    eBC1_UNORM, // DXGI_FORMAT_BC1_UNORM (DXT1)
    eBC2_UNORM, // DXGI_FORMAT_BC2_UNORM (DXT3)
    eBC3_UNORM, // DXGI_FORMAT_BC3_UNORM (DXT5)
    eBC4_UNORM,
    eBC5_UNORM,
    eBC7_UNORM,
    eBC1_UNORM_SRGB,
    eBC2_UNORM_SRGB,
    eBC3_UNORM_SRGB,
    eBC7_UNORM_SRGB,

    // --- Typeless Formats (주로 Depth buffer에서 SRV와 DSV 동시 사용 시) ---
    // 리소스 자체는 Typeless로 만들고, View 생성 시 구체적인 Format 지정
    // 이 포맷 자체를 TextureFormat에 넣기보다는, TextureDesc 플래그나 별도 필드로
    // View용 포맷을 지정하는 방식이 일반적
    // R32_TYPELESS,
    // R24G8_TYPELESS,
    // R16_TYPELESS,

    eMax_TextureFormats
};

enum class ETextureUsage : uint16_t
{
    eNone = 0,

    eShaderResource = 1,
    // 렌더 타겟으로 사용 가능 (컬러 버퍼)
    eRenderTarget = 1 << 1,
    // 깊이/스텐실 버퍼로 사용 가능
    eDepthStencil = 1 << 2,

    eRenderTargetShaderResource = 1 << 3 // 렌더타켓 + 셰이더리소스로 사용
};

// 원본해상도(밉맵레벨0)정보
struct TexMetaData
{
    size_t mWidth;
    size_t mHeight;    // Should be 1 for 1D textures
    size_t mDepth;     // Should be 1 for 1D or 2D textures
    size_t mArraySize; // For cubemap, this is a multiple of 6
    size_t mMipLevels;
    uint32_t mMiscFlags;
    uint32_t mMiscFlags2;
    ETextureFormat mFormat;
    ETextureType mDimension;

    bool operator==(const TexMetaData &texMetaData) const = default;
    bool operator!=(const TexMetaData &texMetaData) const = default;
};

// 각 조각들(이미지배열의원소,밉맵레벨3)정보
struct Image
{
    uint64_t mWidth;
    uint64_t mHeight;
    ETextureFormat mFormat;
    uint64_t mRowPitch;
    uint64_t mSlicePitch;
    uint8_t *mPixels;
};

struct ScratchImage
{
    uint64_t mimagesNum;
    uint64_t mSize; // 전체픽셀사이즈
    TexMetaData mMetadata;
    std::vector<Image> mImages;
    std::vector<uint8_t> mMemory;
    inline ScratchImage() = default;
    inline ~ScratchImage() = default;
    inline ScratchImage(ScratchImage &&) = default;
    inline ScratchImage &operator=(ScratchImage &&) = default;

    inline ScratchImage(const ScratchImage &rhs)
    {
        mimagesNum = rhs.mimagesNum;
        mSize = rhs.mSize;
        mMetadata = rhs.mMetadata;

        mImages.resize(mimagesNum);
        mMemory.resize(mSize);

        for (int i = 0; i < rhs.mimagesNum; ++i)
        {
            mImages[i] = rhs.mImages[i];

            // offset
            uintptr_t offset = rhs.mImages[i].mPixels - rhs.mMemory.data();
            mImages[i].mPixels = mMemory.data() + offset;
        }
        memcpy(mMemory.data(), rhs.mMemory.data(), mSize);
    }

    inline ScratchImage &operator=(const ScratchImage &rhs)
    {
        mimagesNum = rhs.mimagesNum;
        mSize = rhs.mSize;
        mMetadata = rhs.mMetadata;

        mImages.resize(mimagesNum);
        mMemory.resize(mSize);
        for (int i = 0; i < rhs.mimagesNum; ++i)
        {
            mImages[i] = rhs.mImages[i];

            // offset
            uintptr_t offset = rhs.mImages[i].mPixels - rhs.mMemory.data();
            mImages[i].mPixels = mMemory.data() + offset;
        }

        memcpy(mMemory.data(), rhs.mMemory.data(), mSize);

        return *this;
    }
};

struct TextureDesc
{
    ETextureUsage mTextureUsage;
    ScratchImage mScratchImage;

    union OptimizedClearValue
    {
        DepthStecnil mOptimizedDepthStencilValue;
        float mOptimizedColor[4] = {0, 0, 0, 0};
    } mOptimizedClearValue;

    bool hasOptimizedClearValue = false;
};

inline bool IsSRGBType(ETextureFormat format)
{
    switch (format)
    {
    case ETextureFormat::eR8G8B8A8_UNORM_SRGB:
    case ETextureFormat::eB8G8R8A8_UNORM_SRGB:
    case ETextureFormat::eBC1_UNORM_SRGB:
    case ETextureFormat::eBC2_UNORM_SRGB:
    case ETextureFormat::eBC3_UNORM_SRGB:
    case ETextureFormat::eBC7_UNORM_SRGB:
        return true;

    default:
        return false;
    }
}

} // namespace GRM