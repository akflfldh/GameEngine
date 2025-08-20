#include "CoreAsset/Texture.h"
#include <D3DGpuResourceManager/IGpuResource.h>

CoreAsset::Texture::Texture(AssetID id, const std::string &name) : Asset(id, name, CoreAsset::EAssetType::eTexture) {}

CoreAsset::Texture::~Texture() {}

void CoreAsset::Texture::SetGpuResource(const GRM::GRMPtr &gpuResource)
{
    // 오직 한번만호출하거나, 교체할때만 호출될수있게될것이다.

    mGpuResource = gpuResource;
}

GRM::IGpuResource *CoreAsset::Texture::GetGpuResource() const
{
    return mGpuResource.getResource();
}

void CoreAsset::Texture::SetRawData(std::unique_ptr<GRM::TextureDesc> rawData)
{

    mRawData = std::move(rawData);
}

const GRM::TextureDesc *CoreAsset::Texture::GetRawData() const
{
    return mRawData.get();
}

const CoreAsset::TextureProperties &CoreAsset::Texture::GetProperties() const
{
    return mProperties;
}
