#include "MapStorer.h"
#include <Core/Map.h>
#include <Core/MapMetaDataHeader.h>
#include <CoreAsset/Asset.h>
Core::MapStorer *Core::MapStorer::GetInstance()
{
    static MapStorer instance;
    return &instance;
}

Core::MapStorer::MapStorer() {}

Core::MapStorer::~MapStorer() {}

void Core::MapStorer::StoreAssetFile(Arch &arch, CoreAsset::Asset *asset, CoreAsset::AssetMetaData *assetMetaData)
{
    if (asset == nullptr)
        return;

    // texture header serialize 가능
    MapCommonHeader mapCommonHeader;
    MapMetaData *mapMetaData = static_cast<MapMetaData *>(assetMetaData);
    // mapCommonHeader = mapMetaData->mRawFileName;

    mapCommonHeader.Serialize(arch);

    // 이거를 메타데이터로 직렬화로 옮기수도있고, 아니면 다르게분리할수도있고 필드는같은데

    asset->Serialize(arch);
}

bool Core::MapStorer::Store(CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData, const std::string &filePath)
{
    return false;
}

bool Core::MapStorer::StoreAssetRawDataFile(Arch &arch, CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData)
{

    Map *map = static_cast<Map *>(asset);
    if (map == nullptr)
        return false;

    map->SerilaizeRawData(arch);

    return true;
}
