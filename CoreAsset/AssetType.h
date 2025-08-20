#pragma once

#include <D3DGpuResourceManager/GpuTypes.h>
#include <string>

namespace CoreAsset
{
using AssetID = uint64_t;
#define NoneAssetID 0

enum class EAssetType : uint32_t
{
    eUnknown = 0,
    eTexture = 0x1 << 1,
    eMesh = 0x1 << 2,
    eMaterial = 0x1 << 3,
    eShader = 0x1 << 4,
    eAnimation = 0x1 << 5,
    eSound = 0x1 << 6,
    ePrefab = 0x1 << 7,
    eCount = 0x1 << 8
};

// 이설정들보고 렌더시스템이 샘플러를 동적으로 만들어서 바인딩하거나,기존의 샘플러를 바인딩할것이다.
enum class EWrapMode
{
    eClamp,
    eRepeat,
    eMirror
}; // 머터리얼속성
enum class EFilterMode
{
    ePoint,
    eLinear,
    eTrilinear
}; // 머터리얼속성

struct CreationTextureAssetInfo
{
    GRM::TextureDesc mTextureDesc;
    std::string mName;
};

struct TextureProperties
{

    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mMipLevels;

    GRM::ETextureFormat mTextureFormat;
    GRM::ETextureType mTextureType;
    bool mIsSRGB; // 감마보정을해야하는지 +(일반 컬러텍스처인지, 데이터텍스처인지도 나타낸다(대략적으로));
};

} // namespace CoreAsset

namespace std
{

template <> struct hash<CoreAsset::EAssetType>
{
    size_t operator()(const CoreAsset::EAssetType &assetType) const
    {
        return static_cast<const uint32_t>(assetType);
    }
};

} // namespace std