#include "CoreAsset/Material.h"

CoreAsset::Material::Material(AssetID assetID, const std::string &name) : Asset(assetID, name, EAssetType::eMaterial) {}

CoreAsset::Material::~Material() {}

const std::vector<CoreAsset::AssetMaterialTexResourceContext> &CoreAsset::Material::GetTexResourceContextList() const
{
    return mTexResourceContextList;
    // TODO: 여기에 return 문을 삽입합니다.
}

void CoreAsset::Material::SetTextureResource(int index, Texture *tex)
{

    mTexResourceContextList[index].mTexture = tex;
}

void CoreAsset::Material::SetSamplerResource(int index, uint32_t samplerID)
{

    mSamplerResourceContextList[index] = samplerID;
}

const std::vector<uint32_t> &CoreAsset::Material::GetSamplerResourceContextList() const
{

    return mSamplerResourceContextList;
}
