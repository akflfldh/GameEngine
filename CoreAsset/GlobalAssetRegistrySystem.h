#pragma once

#include "CoreAsset/AssetIDGenerator.h"
#include "CoreAsset/AssetIDTable.h"
#include "CoreAsset/AssetPtr.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
#include <vector>

namespace CoreAsset
{

class Asset;
class CORE_ASSET_API GlobalAssetRegistrySystem
{
  public:
    static GlobalAssetRegistrySystem *GetInstance();
    GlobalAssetRegistrySystem();
    ~GlobalAssetRegistrySystem();

    Asset *GetAsset(AssetID id) const;
    Asset *GetAsset(const std::string &name) const;

    AssetID GetNextAssetID();

    // 등록하기전 assetID를 asset에 설정해아한다.
    bool RegisterAsset(Asset *asset, const std::string &name);

    bool HasName(const std::string &name) const;

    void AddDirtyAsset(Asset *asset);

    const std::vector<AssetPtr> &GetDirtyAssetList() const;

    void SetNextAssetID(AssetID id);
    AssetID PeekNextAssetID() const;

  private:
    AssetIDTable mAssetTable;
    AssetIDGenerator mAssetIDGenerator;

    std::vector<AssetPtr> mDirtyAssetList;
};

} // namespace CoreAsset