#pragma once
#include <Core/CoreDllExport.h>
#include <CoreAsset/AssetStorer.h>

class Arch;

namespace Core
{

class CORE_API_LIB MapStorer : public CoreAsset::AssetStorer
{
  public:
    static MapStorer *GetInstance();

    MapStorer();
    virtual ~MapStorer();

    virtual void StoreAssetFile(Arch &arch, CoreAsset::Asset *asset, CoreAsset::AssetMetaData *assetMetaData) override;

    virtual bool Store(CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData,
                       const std::string &filePath) override;
    virtual bool StoreAssetRawDataFile(Arch &arch, CoreAsset::Asset *asset,
                                       CoreAsset::AssetMetaData *metaData) override;

  private:
};

} // namespace Core