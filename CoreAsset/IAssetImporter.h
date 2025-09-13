#pragma once

#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreBase/FVector.h>

namespace CoreAsset
{

struct IntermediateAsset;
class AssetImporterManager;
class CORE_ASSET_API IAssetImporter
{
  public:
    IAssetImporter();
    virtual ~IAssetImporter() = 0;

    virtual FVector<IntermediateAsset *> Import(const char *filePath, AssetImporterManager *importerManager) const = 0;

  private:
};
} // namespace CoreAsset
