#pragma once

#include "CoreAsset/Asset.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
// #include <D3DGpuResourceManager/GRMPtr.h>
// #include <D3DGpuResourceManager/GpuTypes.h>
#include <memory>
namespace GRM
{
class IGpuResource;
}

namespace CoreAsset
{

class CORE_ASSET_API Texture : public Asset
{

    friend class TextureManager;
    friend class TextureFactory;

  public:
    virtual ~Texture();

    static EAssetType GetAssetType()
    {
        return EAssetType::eTexture;
    }

    uint32_t GetWidth() const
    {
        return mProperties.mMetaData.mScratchImage.mMetadata.mWidth;
    }
    uint32_t GetHeight() const
    {
        return mProperties.mMetaData.mScratchImage.mMetadata.mHeight;
    }
    uint32_t GetMipLevels() const
    {
        return mProperties.mMetaData.mScratchImage.mMetadata.mMipLevels;
    }
    GRM::ETextureFormat GetTextureFormat() const
    {
        return mProperties.mMetaData.mScratchImage.mMetadata.mFormat;
    }
    GRM::ETextureType GetTextureType() const
    {
        return mProperties.mMetaData.mScratchImage.mMetadata.mDimension;
    } // 여기서 GRM::TextureType 사용
    bool IsSRGB() const
    {
        return mProperties.mIsSRGB;
    }

    const uint8_t *GetRawData() const;

    const TextureProperties &GetProperties() const;
    TextureProperties &GetProperties();
    // 멀티플레이시 서버의 경우에는 렌더에관련해서 gpuResource리소스가전혀필요없다, 의존성을 없애야한다. asset은 순수한
    // 데이터덩어리
    //     GRM::IGpuResource *GetGpuResource() const;

    virtual void Serialize(Arch &arch) override;

  private:
    Texture();

    void SetSize(uint32_t width, uint32_t height)
    {
        mProperties.mMetaData.mScratchImage.mMetadata.mWidth = width;
        mProperties.mMetaData.mScratchImage.mMetadata.mHeight = height;
    }
    void SetMipLevels(uint32_t levels)
    {
        mProperties.mMetaData.mScratchImage.mMetadata.mMipLevels = levels;
    }
    void SetTextureFormat(GRM::ETextureFormat format)
    {
        mProperties.mMetaData.mScratchImage.mMetadata.mFormat = format;
    }
    void SetTextureType(GRM::ETextureType type)
    {
        mProperties.mMetaData.mScratchImage.mMetadata.mDimension = type;
    }
    void SetSRGB(bool srgb)
    {
        mProperties.mIsSRGB = srgb;
    }

    // 일반적으로 editor에서 사용
    void SetRawData(uint8_t *pMemory, size_t size);

    void SetTextureDesc(const GRM::TextureDesc &texDesc);

  private:
    TextureProperties mProperties;
};
} // namespace CoreAsset
