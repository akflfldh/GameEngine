#pragma once

#include "CoreAsset/AssetMetaDataType.h"
#include "CoreAsset/BaseAssetStorer.h"
#include "CoreAsset/CoreAssetDLLMacro.h"

namespace CoreAsset
{
class Material;
class CORE_ASSET_API MaterialStorer : public BaseAssetStorer
{
  public:
    static MaterialStorer *GetInstance();
    ~MaterialStorer();

    bool Store(CoreAsset::Material *asset, CoreAsset::AssetMetaData *metaData, const std::string &filePath);

  private:
    MaterialStorer();
};

} // namespace CoreAsset