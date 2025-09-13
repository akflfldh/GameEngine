#include "AssetFactoryManager.h"
#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/GlobalAssetRegistrySystem.h>
#include <CoreAsset/IAssetFactory.h>
#include <CoreAsset/IntermediateAsset.h>
CoreAsset::AssetFactoryManager *CoreAsset::AssetFactoryManager::GetInstance()
{

    static AssetFactoryManager instance;
    return &instance;
}

CoreAsset::AssetFactoryManager::AssetFactoryManager() {}

CoreAsset::AssetFactoryManager::~AssetFactoryManager() {}

void CoreAsset::AssetFactoryManager::ReigsterAssetFactory(EAssetType assetType, IAssetFactory *assetFactory)
{

    mAssetFactoryTable[assetType] = assetFactory;
}

CoreAsset::Asset *CoreAsset::AssetFactoryManager::CreateEmptyAsset(const AssetMetaData &assetMetaData)
{

    return mAssetFactoryTable[assetMetaData.mAssetType]->CreateEmptyAsset();
}
CoreAsset::Asset *CoreAsset::AssetFactoryManager::CreateEmptyAsset(EAssetType assetType)
{
    return mAssetFactoryTable[assetType]->CreateEmptyAsset();
}
CoreAsset::Asset *CoreAsset::AssetFactoryManager::CreateAssetFromData(const IntermediateAsset &intermediateAsset)
{
    std::unordered_map<EAssetType, IAssetFactory *>::iterator it =
        mAssetFactoryTable.find(intermediateAsset.mAssetType);
    if (it == mAssetFactoryTable.end())
        return nullptr;

    Asset *asset = it->second->CreateAssetFromData(intermediateAsset);

    if (asset == nullptr)
    {
        // asset 생성 실패
        return nullptr;
    }

    return asset;
}
