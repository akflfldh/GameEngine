#pragma once
#include <CoreAsset/AssetStorer.h>

namespace CoreAsset
{

class CORE_ASSET_API MeshStorer : public AssetStorer
{

  public:
    static MeshStorer *GetInstance();
    MeshStorer();
    virtual ~MeshStorer();

    virtual void StoreAssetFile(Arch &arch, Asset *asset, AssetMetaData *assetMetaData) override;

    virtual bool Store(CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData,
                       const std::string &filePath) override;
    virtual bool StoreAssetRawDataFile(Arch &arch, CoreAsset::Asset *asset,
                                       CoreAsset::AssetMetaData *metaData) override;

  private:
};
} // namespace CoreAsset
