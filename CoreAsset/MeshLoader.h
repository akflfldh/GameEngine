#pragma once

#include <CoreAsset/AssetLoader.h>

namespace CoreAsset
{
class CORE_ASSET_API MeshLoader : public AssetLoader
{
  public:
    static MeshLoader *GetInstance();
    MeshLoader();
    virtual ~MeshLoader();
    virtual bool LoadAssetFile(EAssetType assetType, Arch &arch, AssetFactoryManager *assetFactoryManage,
                               Asset *&oAsset, std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr) override;

    virtual bool LoadAssetRawFile(Arch &arch, Asset *asset) override;

  private:
};

} // namespace CoreAsset