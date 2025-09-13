#pragma once

#include "CoreAsset/CoreAssetDLLMacro.h"
#include <BinaryReaderWriter/BinaryWriter.h>
#include <fstream>
#include <vector>

class Arch;

namespace CoreAsset
{
class Asset;
class AssetMetaData;

class CORE_ASSET_API AssetStorer
{
  public:
    AssetStorer();
    virtual ~AssetStorer() = 0;

    virtual void StoreAssetFile(Arch &arch, Asset *asset, AssetMetaData *assetMetaData) = 0;

    virtual bool Store(CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData, const std::string &filePath) = 0;
    virtual bool StoreAssetRawDataFile(Arch &arch, CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData) = 0;

  protected:
    QuadRW::BinaryWriter mWriter;
};

} // namespace CoreAsset
