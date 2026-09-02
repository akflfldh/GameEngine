#pragma once

#include <CoreAsset/AssetPtr.h>
#include <CoreAsset/AssetType.h>
#include <CoreAsset/CoreAssetDLLMacro.h>
#include <unordered_map>
namespace CoreAsset
{
class IAssetFactory;
struct IntermediateAsset;

class AssetMetaData;
class GlobalAssetRegistrySystem;

class CORE_ASSET_API AssetFactoryManager
{

  public:
    static AssetFactoryManager *GetInstance();
    ~AssetFactoryManager();

    void RegisterAssetFactory(EAssetType assetType, IAssetFactory *assetFactory);

    Asset *CreateEmptyAsset(const AssetMetaData &assetMetaData);
    Asset *CreateEmptyAsset(EAssetType assetType);
    Asset *CreateAssetFromData(const IntermediateAsset &intermediateAsset);

  private:
    AssetFactoryManager();

  private:
    GlobalAssetRegistrySystem *mGlobalAssetRegistrySystem;

    std::unordered_map<EAssetType, IAssetFactory *> mAssetFactoryTable;
};

} // namespace CoreAsset