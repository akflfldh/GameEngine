#pragma once

#include "CoreAsset/AssetType.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
#include <string>
#include <unordered_map>

namespace CoreAsset
{
class Asset;

class CORE_ASSET_API AssetIDTable
{
  public:
    AssetIDTable();
    ~AssetIDTable();

    bool HasName(const std::string &name) const;
    bool HasID(AssetID id) const;

    AssetID GetAssetID(const std::string &name) const;
    const std::string GetAssetName(AssetID id) const;
    Asset *GetAsset(AssetID id) const;
    Asset *GetAsset(const std::string &name) const;

    void GetAssetsByType(EAssetType type, std::vector<Asset *> &oAssetList) const;

    bool SetAsset(const std::string &name, AssetID id, Asset *asset);

    size_t GetAssetNum() const;

  private:
    using AssetNameIDTableType = std::unordered_map<std::string, AssetID>;

    // id ,assetName
    using AssetIDNameTableType = std::unordered_map<AssetID, std::string>;
    // id asset
    using AssetIDPtrTableType = std::unordered_map<AssetID, Asset *>;

    AssetNameIDTableType mNameIDTable;
    AssetIDNameTableType mIDNameTable;
    AssetIDPtrTableType mIDPtrTable;
};

} // namespace CoreAsset