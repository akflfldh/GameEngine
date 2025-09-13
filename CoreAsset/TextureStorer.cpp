#include "CoreAsset/TextureStorer.h"
#include "CoreAsset/AssetCommon.h"
#include "CoreAsset/AssetMetaDataType.h"
#include "CoreAsset/Texture.h"
#include <fstream>

CoreAsset::TextureStorer *CoreAsset::TextureStorer::GetInstance()
{
    static TextureStorer instance;
    return &instance;
}

CoreAsset::TextureStorer::TextureStorer() {}

CoreAsset::TextureStorer::~TextureStorer() {}

void CoreAsset::TextureStorer::StoreAssetFile(Arch &arch, Asset *asset, AssetMetaData *assetMetaData)
{
    if (asset == nullptr)
        return;

    // texture header serialize 가능
    TextureCommonHeader textureCommonHeader;
    TextureMetaData *textureMetaData = static_cast<TextureMetaData *>(assetMetaData);
    textureCommonHeader.mRawFileName = textureMetaData->mRawFileName;

    textureCommonHeader.Serialize(arch);

    // 이거를 메타데이터로 직렬화로 옮기수도있고, 아니면 다르게분리할수도있고 필드는같은데

    asset->Serialize(arch);
}

bool CoreAsset::TextureStorer::Store(CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData,
                                     const std::string &filePath)
{
    return false;
}

bool CoreAsset::TextureStorer::StoreAssetRawDataFile(Arch &arch, CoreAsset::Asset *asset,
                                                     CoreAsset::AssetMetaData *metaData)
{

    Texture *texture = static_cast<Texture *>(asset);

    const TextureProperties &properties = texture->GetProperties();
    const uint8_t *pMem = properties.mMetaData.mScratchImage.mMemory.data();
    // 저장이라는것이 확실하기에 const 캐스팅 가능
    arch << QUAD_SERIALIZEBUFFER(const_cast<uint8_t *>(pMem), properties.mMetaData.mScratchImage.mSize);

    return true;
}
