#pragma once

#include "CoreAsset/AssetMetaDataType.h"
#include "CoreAsset/AssetStorer.h"
#include "CoreAsset/CoreAssetDLLMacro.h"

namespace CoreAsset
{
class Material;
class CORE_ASSET_API MaterialStorer : public AssetStorer
{
  public:
    static MaterialStorer *GetInstance();
    ~MaterialStorer();

    virtual void StoreAssetFile(Arch &arch, Asset *asset, AssetMetaData *assetMetaData) override;

    virtual bool Store(CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData,
                       const std::string &filePath) override;

    virtual bool StoreAssetRawDataFile(Arch &arch, CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData
                                       ) override;

  private:
    MaterialStorer();
};

} // namespace CoreAsset