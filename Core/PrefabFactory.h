#pragma once

#include <Core/CoreDllExport.h>
#include <CoreAsset/IAssetFactory.h>

class CORE_API_LIB PrefabFactory : public CoreAsset::IAssetFactory
{
  public:
    static PrefabFactory *GetInstance();

    PrefabFactory();
    ~PrefabFactory();

    virtual CoreAsset::Asset *CreateEmptyAsset(CoreAsset::EAssetType assetType) override;
    virtual CoreAsset::Asset *CreateAssetFromData(const CoreAsset::IntermediateAsset &serializedAsset) override;

  private:
};
