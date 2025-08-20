#pragma once

#include "CoreAsset/Asset.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
#include <D3DGpuResourceManager/GRMPtr.h>
#include <D3DGpuResourceManager/GpuTypes.h>
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

  public:
    virtual ~Texture();

    uint32_t GetWidth() const
    {
        return mProperties.mWidth;
    }
    uint32_t GetHeight() const
    {
        return mProperties.mHeight;
    }
    uint32_t GetMipLevels() const
    {
        return mProperties.mMipLevels;
    }
    GRM::ETextureFormat GetTextureFormat() const
    {
        return mProperties.mTextureFormat;
    }
    GRM::ETextureType GetTextureType() const
    {
        return mProperties.mTextureType;
    } // 여기서 GRM::TextureType 사용
    bool IsSRGB() const
    {
        return mProperties.mIsSRGB;
    }

    const GRM::TextureDesc *GetRawData() const;

    const TextureProperties &GetProperties() const;

    GRM::IGpuResource *GetGpuResource() const;

  private:
    Texture(AssetID id, const std::string &name); //
    void SetGpuResource(const GRM::GRMPtr &gpuResource);

    void SetSize(uint32_t width, uint32_t height)
    {
        mProperties.mWidth = width;
        mProperties.mHeight = height;
    }
    void SetMipLevels(uint32_t levels)
    {
        mProperties.mMipLevels = levels;
    }
    void SetTextureFormat(GRM::ETextureFormat format)
    {
        mProperties.mTextureFormat = format;
    }
    void SetTextureType(GRM::ETextureType type)
    {
        mProperties.mTextureType = type;
    } // 여기서 GRM::TextureType 사용
    void SetSRGB(bool srgb)
    {
        mProperties.mIsSRGB = srgb;
    }

    // 일반적으로 editor에서 사용
    void SetRawData(std::unique_ptr<GRM::TextureDesc> rawData);

  private:
    GRM::GRMPtr mGpuResource;
    TextureProperties mProperties;

    std::unique_ptr<GRM::TextureDesc> mRawData;
};
} // namespace CoreAsset
