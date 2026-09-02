#include "MapFactory.h"
#include <Core/CoreType.h>
#include <Core/Map.h>
Core::MapFactory *Core::MapFactory::GetInstance()
{
    static MapFactory instance;

    return &instance;
}

Core::MapFactory::MapFactory() {}

Core::MapFactory::~MapFactory() {}

CoreAsset::Asset *Core::MapFactory::CreateEmptyAsset(CoreAsset::EAssetType assetType)
{
    if (assetType == CoreAsset::EAssetType::eMap)
    {
        return CreateMapInstance();
    }

    return nullptr;
}

CoreAsset::Asset *Core::MapFactory::CreateAssetFromData(const CoreAsset::IntermediateAsset &intermedidateAsset)
{
    const Core::IntermediateMap &interemdiateMap = static_cast<const Core::IntermediateMap &>(intermedidateAsset);

    if (interemdiateMap.mAssetType == CoreAsset::EAssetType::eMap)
    {
        return CreateMapInstance();
    }

    return nullptr;
}

Map *Core::MapFactory::CreateMapInstance()
{
    return new Map;
}
