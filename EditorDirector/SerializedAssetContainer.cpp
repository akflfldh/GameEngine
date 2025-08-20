#include "EditorDirector/SerializedAssetContainer.h"

Quad::SerializedAssetTContainer *Quad::SerializedAssetTContainer::GetInstance()
{
    static SerializedAssetTContainer instance;
    return &instance;
}

Quad::SerializedAssetTContainer::SerializedAssetTContainer() {}

Quad::SerializedAssetTContainer::~SerializedAssetTContainer() {}

void Quad::SerializedAssetTContainer::Register(
    std::unique_ptr<CoreAsset::SerializedAsset> serializedAsset,
    std::unique_ptr<CoreAsset::SerializedAssetRawData> serializedRawAssetData,
    const CoreAsset::AssetMetaData &assetMetaDataconst, std::string &parentFolderPath)
{

    if (serializedAsset == nullptr)
        return;

    CoreAsset::EAssetType assetType = serializedAsset->mAssetType;
    mSerializedAssetContextTable[assetType].push_back(
        {std::move(serializedAsset), std::move(serializedRawAssetData), assetMetaDataconst, parentFolderPath});
}

void Quad::SerializedAssetTContainer::Register(
    std::unique_ptr<CoreAsset::SerializedAsset> serializedAsset,
    std::unique_ptr<CoreAsset::SerializedAssetRawData> serializedRawAssetData,
    const CoreAsset::AssetMetaData &assetMetaDataconst, std::string &&parentFolderPath)
{
    if (serializedAsset == nullptr)
        return;

    CoreAsset::EAssetType assetType = serializedAsset->mAssetType;
    mSerializedAssetContextTable[assetType].push_back({std::move(serializedAsset), std::move(serializedRawAssetData),
                                                       assetMetaDataconst, std::move(parentFolderPath)});
}

const std::vector<Quad::SerializedAssetContainerContext> &Quad::SerializedAssetTContainer::
    GetSerializedAssetContextVector(CoreAsset::EAssetType assetType)
{
    return mSerializedAssetContextTable[assetType];
}

void Quad::SerializedAssetTContainer::Clear()
{
    mSerializedAssetContextTable.clear();
}