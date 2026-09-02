#pragma once

#include <CoreAsset/AssetLoader.h>

#include <Core/CoreDllExport.h>

class CORE_API_LIB PrefabLoader : public CoreAsset::AssetLoader
{
  public:
    static PrefabLoader *GetInstance();

    PrefabLoader();
    virtual ~PrefabLoader();

    virtual bool LoadAssetFile(CoreAsset::EAssetType assetType, Arch &arch,
                               CoreAsset::AssetFactoryManager *assetFactoryManage, CoreAsset::Asset *&oAsset,
                               std::unique_ptr<CoreAsset::AssetMetaData> &oAssetMetaDataPtr) override;

    virtual bool LoadAssetRawFile(Arch &arch, CoreAsset::Asset *asset) override;

  private:
};
