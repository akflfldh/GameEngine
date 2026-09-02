#include "TextureFactory.h"
#include <CoreAsset/GlobalAssetRegistrySystem.h>
#include <CoreAsset/IntermediateAsset.h>
#include <CoreAsset/Texture.h>
CoreAsset::TextureFactory *CoreAsset::TextureFactory::GetInstance()
{
    static TextureFactory instance;
    return &instance;
}

CoreAsset::TextureFactory::TextureFactory() {}

CoreAsset::TextureFactory::~TextureFactory() {}

CoreAsset::Asset *CoreAsset::TextureFactory::CreateEmptyAsset(EAssetType assetType)
{
    // 임시
    return new Texture;
}

CoreAsset::Asset *CoreAsset::TextureFactory::CreateAssetFromData(const IntermediateAsset &intermediateAsset)
{

    // 중간데이터를 가지고 텍스처 에셋을 생성한다.

    const IntermediateTexture &intermediateTexture = static_cast<const IntermediateTexture &>(intermediateAsset);

    // Texture Asset 생성
    Texture *texture = CreateTexture();
    if (texture == nullptr)
    {
        return nullptr;
    }
    // 메타데이터 설정
    SetPropertyMetaData(texture, intermediateTexture);

    return texture;
}

CoreAsset::Texture *CoreAsset::TextureFactory::CreateTexture()
{

    // 커스텀할당자
    return new Texture;
}

void CoreAsset::TextureFactory::SetPropertyMetaData(Texture *texture, const IntermediateTexture &intermediateTexture)
{

    if (texture == nullptr)
        return;

    texture->SetLoadState(Asset::LoadState::Loaded);

    texture->SetTextureDesc(intermediateTexture.mTextureRawData);
    // texture->SetName(intermediateTexture.mAssetName.c_str());
    //    texture->SetEmptyAssetFlag(true);
}
