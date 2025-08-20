#include "CoreAsset/TextureLoader.h"
#include "CoreAsset/AssetMetaDataType.h"
#include "CoreAsset/MaterialLoader.h"
#include <BinaryReaderWriter/BinaryReader.h>

CoreAsset::TextureLoader *CoreAsset::TextureLoader::GetInstance()
{
    static TextureLoader instance;
    return &instance;
}

CoreAsset::TextureLoader::TextureLoader() {}
CoreAsset::TextureLoader::~TextureLoader() {}

std::unique_ptr<CoreAsset::SerializedTexture> CoreAsset::TextureLoader::LoadAssetFile(
    const std::string &filePath, CoreAsset::AssetMetaData &oMetaData)
{

    QuadRW::BinaryReader binaryReader;

    std::unique_ptr<SerializedTexture> serializedTexture = std::make_unique<SerializedTexture>();

    bool ret = binaryReader.StartRead(filePath);
    if (ret == false)
    {
        return nullptr;
    }

    serializedTexture->DeSerialize(binaryReader);

    bool metaDataValidFlag = false;
    binaryReader.Read(metaDataValidFlag);

    if (metaDataValidFlag == true)
        oMetaData.DeSerialize(binaryReader);

    return serializedTexture;
}

std::unique_ptr<CoreAsset::SerializedTextureRawData> CoreAsset::TextureLoader::LoadRawFile(const std::string &filePath)
{
    QuadRW::BinaryReader binaryReader;

    std::unique_ptr<CoreAsset::SerializedTextureRawData> serializedTextureRawData =
        std::make_unique<SerializedTextureRawData>();

    if (binaryReader.StartRead(filePath) == false)
    {

        // log
        return nullptr;
    }

    serializedTextureRawData->DeSerialize(binaryReader);

    return serializedTextureRawData;
}
