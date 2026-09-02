#include "PrefabLoader.h"
#include <Core/Prefab.h>
#include <CoreAsset/AssetFactoryManager.h>
#include <core/PrefabMetaDataHeader.h>

PrefabLoader *PrefabLoader::GetInstance()
{
    static PrefabLoader instance;

    return &instance;
}

PrefabLoader::PrefabLoader() {}

PrefabLoader::~PrefabLoader() {}

bool PrefabLoader::LoadAssetFile(CoreAsset::EAssetType assetType, Arch &arch,
                                 CoreAsset::AssetFactoryManager *assetFactoryManager, CoreAsset::Asset *&oAsset,
                                 std::unique_ptr<CoreAsset::AssetMetaData> &oAssetMetaDataPtr)
{

    Core::PrefabCommonHeader prefabCommonHeader;

    prefabCommonHeader.Serialize(arch);

    std::unique_ptr<Core::PrefabMetaData> mapMetaDataPtr = std::make_unique<Core::PrefabMetaData>();

    Prefab *prefab = static_cast<Prefab *>(assetFactoryManager->CreateEmptyAsset(CoreAsset::EAssetType::ePrefab));

    if (prefab)
        prefab->Serialize(arch);

    oAsset = prefab;
    oAssetMetaDataPtr = std::move(mapMetaDataPtr);
    return true;
}

bool PrefabLoader::LoadAssetRawFile(Arch &arch, CoreAsset::Asset *asset)
{
    return true;
}
