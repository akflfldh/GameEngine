#include "CoreAsset/MaterialLoader.h"
#include "CoreAsset/AssetMetaDataType.h"
#include <BinaryReaderWriter/BinaryReader.h>
#include <CoreAsset/AssetFactoryManager.h>

CoreAsset::MaterialLoader *CoreAsset::MaterialLoader::GetInstance()
{
    static MaterialLoader instance;
    return &instance;
}
CoreAsset::MaterialLoader::MaterialLoader() {}

CoreAsset::MaterialLoader::~MaterialLoader() {}

bool CoreAsset::MaterialLoader::LoadAssetFile(EAssetType assetType, Arch &arch,
                                              AssetFactoryManager *assetFactoryManager, Asset *&oAsset,
                                              std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr)
{

    MaterialCommonHeader materialCommonHeader;

    materialCommonHeader.Serialize(arch);

    std::unique_ptr<MaterialMetaData> materialMetaDataPtr = std::make_unique<MaterialMetaData>();
    //    materialMetaDataPtr->mRawFileName = materialMetaDataPtr.mRawFileName;
    // materialMetaDataPtr->mAssetType = CoreAsset::EAssetType::eTexture;

    // 그 후 빈에셋(Texture)생성후 Serialize수행
    oAsset = assetFactoryManager->CreateEmptyAsset(EAssetType::eMaterial);
    if (oAsset != nullptr)
        oAsset->Serialize(arch);

    oAssetMetaDataPtr = std::move(materialMetaDataPtr);

    return true;
}

bool CoreAsset::MaterialLoader::LoadAssetRawFile(Arch &arch, CoreAsset::Asset *asset)
{
    return true;
}

// std::unique_ptr<CoreAsset::SerializedAsset> CoreAsset::MaterialLoader::LoadAssetFile(
//     const std::string &filePath, CoreAsset::AssetMetaData &oMetaData)
//{
//
//     /*QuadRW::BinaryReader binaryReader;
//
//     std::unique_ptr<SerializedMaterial> serializedMaterial = std::make_unique<SerializedMaterial>();
//
//     bool ret = binaryReader.StartRead(filePath);
//     if (ret == false)
//     {
//         return nullptr;
//     }
//
//     serializedMaterial->DeSerialize(binaryReader);
//
//     bool metaDataValidFlag = false;
//     binaryReader.Read(metaDataValidFlag);
//
//     if (metaDataValidFlag == true)
//         oMetaData.DeSerialize(binaryReader);
//
//     return serializedMaterial;*/
//
//     return nullptr;
// }

// std::unique_ptr<CoreAsset::AssetMetaData> CoreAsset::MaterialLoader::LoadAssetMetaDataFile(
//     QuadRW::BinaryReader &binaryReader)
//{
//
//     return std::unique_ptr<AssetMetaData>();
// }
//
// std::unique_ptr<CoreAsset::SerializedAssetRawData> CoreAsset::MaterialLoader::LoadRawFile(const std::string
// &filePath)
//{
//     return nullptr;
// }
