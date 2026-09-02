#include "MapLoader.h"
#include <Core/Map.h>
#include <Core/MapMetaDataHeader.h>
#include <CoreAsset/AssetFactoryManager.h>

Core::MapLoader *Core::MapLoader::GetInstance()
{
    static MapLoader instance;
    return &instance;
}

Core::MapLoader::MapLoader() {}

Core::MapLoader::~MapLoader() {}

bool Core::MapLoader::LoadAssetFile(CoreAsset::EAssetType assetType, Arch &arch,
                                    CoreAsset::AssetFactoryManager *assetFactoryManager, CoreAsset::Asset *&oAsset,
                                    std::unique_ptr<CoreAsset::AssetMetaData> &oAssetMetaDataPtr)
{

    MapCommonHeader mapCommonHeader;

    mapCommonHeader.Serialize(arch);

    std::unique_ptr<MapMetaData> mapMetaDataPtr = std::make_unique<MapMetaData>();
    //    materialMetaDataPtr->mRawFileName = materialMetaDataPtr.mRawFileName;
    // materialMetaDataPtr->mAssetType = CoreAsset::EAssetType::eTexture;

    // 그 후 빈에셋(Texture)생성후 Serialize수행
    oAsset = assetFactoryManager->CreateEmptyAsset(CoreAsset::EAssetType::eMap);
    if (oAsset != nullptr)
        oAsset->Serialize(arch);

    oAssetMetaDataPtr = std::move(mapMetaDataPtr);

    return true;
}

bool Core::MapLoader::LoadAssetRawFile(Arch &arch, CoreAsset::Asset *asset)
{

    Map *map = static_cast<Map *>(asset);
    if (map == nullptr)
        return false;

    map->SerilaizeRawData(arch);
    return true;
}
