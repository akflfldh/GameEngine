#pragma once

#include <Core/CoreDllExport.h>
#include <CoreAsset/AssetLoader.h>

namespace Core
{

class CORE_API_LIB MapLoader : public CoreAsset::AssetLoader
{
  public:
    static MapLoader *GetInstance();

    MapLoader();
    virtual ~MapLoader();

    virtual bool LoadAssetFile(CoreAsset::EAssetType assetType, Arch &arch,
                               CoreAsset::AssetFactoryManager *assetFactoryManage, CoreAsset::Asset *&oAsset,
                               std::unique_ptr<CoreAsset::AssetMetaData> &oAssetMetaDataPtr) override;

    virtual bool LoadAssetRawFile(Arch &arch, CoreAsset::Asset *asset) override;

  private:
};

} // namespace Core
