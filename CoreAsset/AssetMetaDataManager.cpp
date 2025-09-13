#include "CoreAsset/AssetMetaDataManager.h"
// #include <CoreAsset/GlobalAssetRegistrySystem.h>
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

bool CoreAsset::AssetMetaDataManager::Register(Asset *asset)
{
    if (asset == nullptr)
        return false;

    switch (asset->GetType())
    {
    case EAssetType::eTexture:
    {
        std::string rawDataFileName = asset->GetName().c_str();

        std::string id = std::to_string(asset->GetID());

        rawDataFileName += id; // 유일성보장

        TextureMetaData textureMetaData;
        textureMetaData.mAssetID = asset->GetID();
        textureMetaData.mAssetName = asset->GetName().c_str();
        textureMetaData.mAssetType = asset->GetType();
        textureMetaData.mKeepRawDataFlag = true;
        textureMetaData.mRawFileName = rawDataFileName.c_str();

        bool ret = Register(textureMetaData);
        return ret;
    }
    break;

    case EAssetType::eMaterial:
    {
        MaterialMetaData materialMetaData;
        materialMetaData.mAssetID = asset->GetID();
        materialMetaData.mAssetName = asset->GetName().c_str();
        materialMetaData.mAssetType = asset->GetType();

        bool ret = Register(materialMetaData);
        return ret;
    }
    break;

    case EAssetType::eMesh:
    {
    }
    break;
    }

    return false;
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

CoreAsset::AssetMetaData *CoreAsset::AssetMetaDataManager::GetMetaData(Asset *asset) const
{

    if (asset == nullptr)
        return nullptr;

    return GetMetaData(asset->GetID());
}
