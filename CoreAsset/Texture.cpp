#include "CoreAsset/Texture.h"
// #include <D3DGpuResourceManager/IGpuResource.h>

CoreAsset::Texture::Texture() : Asset(CoreAsset::EAssetType::eTexture) {}

CoreAsset::Texture::~Texture() {}

void CoreAsset::Texture::Serialize(Arch &arch)
{
    Asset::Serialize(arch);

    arch << mProperties;
}

void CoreAsset::Texture::SetRawData(uint8_t *pMemory, size_t size)
{
    mProperties.SetRawData(pMemory, size);
}

void CoreAsset::Texture::SetTextureDesc(const GRM::TextureDesc &texDesc)
{
    mProperties.mMetaData = texDesc;
}

const uint8_t *CoreAsset::Texture::GetRawData() const
{
    return mProperties.mMetaData.mScratchImage.mMemory.data();
}

const CoreAsset::TextureProperties &CoreAsset::Texture::GetProperties() const
{
    return mProperties;
}

CoreAsset::TextureProperties &CoreAsset::Texture::GetProperties()
{
    return mProperties;
    // TODO: 여기에 return 문을 삽입합니다.
}
