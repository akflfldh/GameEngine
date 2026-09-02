#include <CoreAsset/AssetMetaDataManager.h>

// #include <CoreAsset/GlobalAssetRegistrySystem.h>
CoreAsset::AssetMetaDataManager *CoreAsset::AssetMetaDataManager::GetInstance()
{
    static AssetMetaDataManager instance;

    return &instance;
}

CoreAsset::AssetMetaDataManager::AssetMetaDataManager() {}

void CoreAsset::AssetMetaDataManager::SetRawFileName(AssetMetaData *metaData)
{

    metaData->mRawFileName = (metaData->mAssetName + std::to_string(metaData->mAssetID)).c_str();
}

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
    break;
    case EAssetType::eStaticMesh:

    {
        MeshMetaData *meshMetaData = new MeshMetaData;
        *meshMetaData = static_cast<const MeshMetaData &>(assetMetaData);

        mAssetMetaDataTable[assetMetaData.mAssetID] = meshMetaData;
    }
    break;
    case EAssetType::eFont:
    {
        FontMetaData *fontMetaData = new FontMetaData;
        *fontMetaData = static_cast<const FontMetaData &>(assetMetaData);

        mAssetMetaDataTable[assetMetaData.mAssetID] = fontMetaData;
    }
    break;
    case EAssetType::eMap:
    {
        MapMetaData *mapMetaData = new MapMetaData;
        *mapMetaData = static_cast<const MapMetaData &>(assetMetaData);

        mAssetMetaDataTable[assetMetaData.mAssetID] = mapMetaData;
    }
    break;
    case EAssetType::ePrefab:
    {
        PrefabMetaData *metaData = new PrefabMetaData;
        *metaData = static_cast<const PrefabMetaData &>(assetMetaData);

        mAssetMetaDataTable[assetMetaData.mAssetID] = metaData;
    }
    break;
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
        TextureMetaData textureMetaData;
        textureMetaData.mAssetID = asset->GetID();
        textureMetaData.mAssetName = asset->GetName().c_str();
        textureMetaData.mAssetType = asset->GetType();
        textureMetaData.mKeepRawDataFlag = true;
        SetRawFileName(&textureMetaData);

        bool ret = Register(textureMetaData);
        return ret;
    }
    break;

    case EAssetType::eMaterial:
    {
        MaterialMetaData metaData;
        metaData.mAssetID = asset->GetID();
        metaData.mAssetName = asset->GetName().c_str();
        metaData.mAssetType = asset->GetType();
        SetRawFileName(&metaData);

        bool ret = Register(metaData);
        return ret;
    }
    break;

    case EAssetType::eStaticMesh:

    {
        MeshMetaData metaData;
        metaData.mAssetID = asset->GetID();
        metaData.mAssetName = asset->GetName().c_str();
        metaData.mAssetType = asset->GetType();
        SetRawFileName(&metaData);
        bool ret = Register(metaData);
        return ret;
    }
    break;

    case EAssetType::eSkinningMesh:
    {
    }
    break;
    case EAssetType::eFont:
    {
        FontMetaData metaData;
        metaData.mAssetID = asset->GetID();
        metaData.mAssetName = asset->GetName().c_str();
        metaData.mAssetType = asset->GetType();
        SetRawFileName(&metaData);

        bool ret = Register(metaData);
        return ret;
    }
    break;
    case EAssetType::eMap:
    {
        MapMetaData metaData;
        metaData.mAssetID = asset->GetID();
        metaData.mAssetName = asset->GetName().c_str();
        metaData.mAssetType = asset->GetType();
        SetRawFileName(&metaData);

        bool ret = Register(metaData);
        return ret;
    }
    break;
    case EAssetType::ePrefab:
    {
        PrefabMetaData metaData;
        metaData.mAssetID = asset->GetID();
        metaData.mAssetName = asset->GetName().c_str();
        metaData.mAssetType = asset->GetType();
        SetRawFileName(&metaData);

        bool ret = Register(metaData);
        return ret;
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
