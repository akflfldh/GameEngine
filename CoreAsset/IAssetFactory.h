#pragma once
#include <CoreAsset/AssetPtr.h>
#include <CoreAsset/CoreAssetDLLMacro.h>
namespace CoreAsset
{
struct IntermediateAsset;
class AssetMetaData;

class CORE_ASSET_API IAssetFactory
{
  public:
    IAssetFactory();
    virtual ~IAssetFactory() = 0;

    virtual Asset *CreateEmptyAsset(EAssetType assetType) = 0;
    virtual Asset *CreateAssetFromData(const IntermediateAsset &serializedAsset) = 0;

    // remove 메서드 향후 추가

  private:
};

} // namespace CoreAsset
