#include "CoreAsset/AssetMetaDataManager.h"

CoreAsset::AssetMetaDataManager *CoreAsset::AssetMetaDataManager::GetInstance()
{
    static AssetMetaDataManager instance;

    return &instance;
}

CoreAsset::AssetMetaDataManager::AssetMetaDataManager() {}

CoreAsset::AssetMetaDataManager::~AssetMetaDataManager() {}

bool CoreAsset::AssetMetaDataManager::Register(const AssetMetaData &assetMetaData)
{

    if (mAssetMetaDataTable.find(assetMetaData.mAssetID) != mAssetMetaDataTable.end())
    {
        return false;
    }

    switch (assetMetaData.mAssetType)
    {
    case EAssetType::eTexture:
    {
        TextureMetaData *textureMetaData = new TextureMetaData;
        *textureMetaData = static_cast<const TextureMetaData &>(assetMetaData);

        mAssetMetaDataTable[assetMetaData.mAssetID] = textureMetaData;
    }
    break;
    case EAssetType::eMaterial:
    {
        AssetMetaData *materialMetaData = new AssetMetaData;
        *materialMetaData = static_cast<const AssetMetaData &>(assetMetaData);

        mAssetMetaDataTable[assetMetaData.mAssetID] = materialMetaData;
    }
    }

    return true;
}

void CoreAsset::AssetMetaDataManager::UnRegister(AssetID id)
{

    std::unordered_map<AssetID, AssetMetaData *>::iterator it = mAssetMetaDataTable.find(id);

    if (it == mAssetMetaDataTable.end())
        return;

    delete it->second;
    mAssetMetaDataTable.erase(it);
}

CoreAsset::AssetMetaData *CoreAsset::AssetMetaDataManager::GetMetaData(AssetID id) const
{

    AssetMetaTable::const_iterator it = mAssetMetaDataTable.find(id);

    if (it == mAssetMetaDataTable.end())
    {
        return nullptr;
    }
    else
    {
        return it->second;
    }
}
