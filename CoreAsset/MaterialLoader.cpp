#include "CoreAsset/MaterialLoader.h"
#include "CoreAsset/AssetMetaDataType.h"
#include <BinaryReaderWriter/BinaryReader.h>

CoreAsset::MaterialLoader *CoreAsset::MaterialLoader::GetInstance()
{
    static MaterialLoader instance;
    return &instance;
}
CoreAsset::MaterialLoader::MaterialLoader() {}

CoreAsset::MaterialLoader::~MaterialLoader() {}

std::unique_ptr<CoreAsset::SerializedMaterial> CoreAsset::MaterialLoader::LoadAssetFile(
    const std::string &filePath, CoreAsset::AssetMetaData &oMetaData)
{

    QuadRW::BinaryReader binaryReader;

    std::unique_ptr<SerializedMaterial> serializedMaterial = std::make_unique<SerializedMaterial>();

    bool ret = binaryReader.StartRead(filePath);
    if (ret == false)
    {
        return nullptr;
    }

    serializedMaterial->DeSerialize(binaryReader);

    bool metaDataValidFlag = false;
    binaryReader.Read(metaDataValidFlag);

    if (metaDataValidFlag == true)
        oMetaData.DeSerialize(binaryReader);

    return serializedMaterial;
}