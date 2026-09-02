#pragma once

#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreAsset/IAssetFactory.h>
namespace CoreAsset
{

class CORE_ASSET_API FontFactory : public IAssetFactory
{
  public:
    static FontFactory *GetInstance();

    virtual Asset *CreateEmptyAsset(EAssetType assetType) override;
    virtual Asset *CreateAssetFromData(const IntermediateAsset &intermediateAsset) override;

  private:
};

} // namespace CoreAsset
