#pragma once

#include "CoreAsset/CoreAssetDLLMacro.h"
#include <CoreAsset/AssetLoader.h>
#include <memory>
#include <string>
namespace CoreAsset
{

class AssetMetaData;
class Asset;

class CORE_ASSET_API TextureLoader : public AssetLoader
{
  public:
    static TextureLoader *GetInstance();
    ~TextureLoader();
    virtual bool LoadAssetFile(EAssetType assetType, Arch &arch, AssetFactoryManager *assetFactoryManager,
                               Asset *&oAsset, std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr) override;
    virtual bool LoadAssetRawFile(Arch &arch, Asset *asse) override;

  protected:
  private:
    TextureLoader();
};

} // namespace CoreAsset
