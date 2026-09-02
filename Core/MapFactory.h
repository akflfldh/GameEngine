#pragma once

#include <Core/CoreDllExport.h>
#include <CoreAsset/IAssetFactory.h>

class Map;

namespace Core
{

class CORE_API_LIB MapFactory : public CoreAsset::IAssetFactory
{
  public:
    static MapFactory *GetInstance();
    MapFactory();
    virtual ~MapFactory();

    virtual CoreAsset::Asset *CreateEmptyAsset(CoreAsset::EAssetType assetType) override;
    virtual CoreAsset::Asset *CreateAssetFromData(const CoreAsset::IntermediateAsset &serializedAsset) override;

  private:
    Map *CreateMapInstance();
};

} // namespace Core