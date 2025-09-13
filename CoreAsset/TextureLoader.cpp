#include "CoreAsset/TextureLoader.h"
#include "CoreAsset/AssetMetaDataType.h"
#include "CoreAsset/MaterialLoader.h"
#include <BinaryReaderWriter/BinaryReader.h>
#include <CoreAsset/Asset.h>
#include <CoreAsset/AssetFactoryManager.h>
#include <CoreAsset/Texture.h>
CoreAsset::TextureLoader *CoreAsset::TextureLoader::GetInstance()
{
    static TextureLoader instance;
    return &instance;
}

CoreAsset::TextureLoader::TextureLoader() {}
CoreAsset::TextureLoader::~TextureLoader() {}

bool CoreAsset::TextureLoader::LoadAssetFile(Arch &arch, AssetFactoryManager *assetFactoryManager, Asset *&oAsset,
                                             std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr)
{

    // texture metaData에대한 texture common header 를 먼저 직렬화
    TextureCommonHeader textureCommonHeader;
    textureCommonHeader.Serialize(arch);

    std::unique_ptr<TextureMetaData> textureMetaDataPtr = std::make_unique<TextureMetaData>();
    textureMetaDataPtr->mRawFileName = textureCommonHeader.mRawFileName;
    textureMetaDataPtr->mAssetType = CoreAsset::EAssetType::eTexture;

    // 그 후 빈에셋(Texture)생성후 Serialize수행
    oAsset = assetFactoryManager->CreateEmptyAsset(EAssetType::eTexture);
    if (oAsset != nullptr)
        oAsset->Serialize(arch);




    oAssetMetaDataPtr = std::move(textureMetaDataPtr);

    return true;
}

bool CoreAsset::TextureLoader::LoadAssetRawFile(Arch &arch, Asset *asset)
{

    CoreAsset::Texture *texture = static_cast<Texture *>(asset);

    uint64_t rawDataSize = texture->GetProperties().mMetaData.mScratchImage.mSize;

    std::vector<uint8_t> pMem(rawDataSize);

    arch << QUAD_SERIALIZEBUFFER(pMem.data(), rawDataSize);

    texture->GetProperties().SetRawData(pMem.data(), rawDataSize);

    return true;
}

// std::unique_ptr<CoreAsset::SerializedAsset> CoreAsset::TextureLoader::LoadAssetFile(const std::string &filePath,
//                                                                                     CoreAsset::AssetMetaData
//                                                                                     &oMetaData)
//{
//
//     QuadRW::BinaryReader binaryReader;
//
//     std::unique_ptr<SerializedTexture> serializedTexture = std::make_unique<SerializedTexture>();
//
//     bool ret = binaryReader.StartRead(filePath);
//     if (ret == false)
//     {
//         return nullptr;
//     }
//
//     serializedTexture->DeSerialize(binaryReader);
//
//     bool metaDataValidFlag = false;
//     binaryReader.Read(metaDataValidFlag);
//
//     if (metaDataValidFlag == true)
//         oMetaData.DeSerialize(binaryReader);
//
//     return serializedTexture;
// }
//
// std::unique_ptr<CoreAsset::AssetMetaData> CoreAsset::TextureLoader::LoadAssetMetaDataFile(
//     QuadRW::BinaryReader &binaryReader)
//{
//     return std::unique_ptr<AssetMetaData>();
// }
//
// std::unique_ptr<CoreAsset::SerializedAssetRawData> CoreAsset::TextureLoader::LoadRawFile(const std::string &filePath)
//{
//     QuadRW::BinaryReader binaryReader;
//
//     std::unique_ptr<CoreAsset::SerializedTextureRawData> serializedTextureRawData =
//         std::make_unique<SerializedTextureRawData>();
//
//     if (binaryReader.StartRead(filePath) == false)
//     {
//
//         // log
//         return nullptr;
//     }
//
//     serializedTextureRawData->DeSerialize(binaryReader);
//
//     return serializedTextureRawData;
// }
