#pragma once

#include "CoreAsset/CoreAssetDLLMacro.h"
#include <BinaryReaderWriter/BinaryWriter.h>
#include <fstream>
#include <vector>
namespace CoreAsset
{
class Asset;
class AssetMetaData;

class CORE_ASSET_API AssetStorer
{
  public:
    AssetStorer();
    virtual ~AssetStorer() = 0;

  protected:
    QuadRW::BinaryWriter mWriter;
};

} // namespace CoreAsset
