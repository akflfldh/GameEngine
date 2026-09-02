#include "PrefabStorer.h"
#include <Core/PrefabMetaDataHeader.h>
#include <CoreAsset/Asset.h>
Core::PrefabStorer *Core::PrefabStorer::GetInstance()
{
    static PrefabStorer instance;
    return &instance;
}

Core::PrefabStorer::PrefabStorer() {}

Core::PrefabStorer::~PrefabStorer() {}

void Core::PrefabStorer::StoreAssetFile(Arch &arch, CoreAsset::Asset *asset, CoreAsset::AssetMetaData *assetMetaData)
{

    if (asset == nullptr)
        return;

    // texture header serialize 가능
    PrefabCommonHeader prefabCommonHeader;
    PrefabMetaData *prefabMetaData = static_cast<PrefabMetaData *>(assetMetaData);
    // mapCommonHeader = mapMetaData->mRawFileName;

    prefabCommonHeader.Serialize(arch);

    // 이거를 메타데이터로 직렬화로 옮기수도있고, 아니면 다르게분리할수도있고 필드는같은데

    asset->Serialize(arch);
}

bool Core::PrefabStorer::Store(CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData, const std::string &filePath)
{
    return false;
}

bool Core::PrefabStorer::StoreAssetRawDataFile(Arch &arch, CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData)
{
    return true;
}
