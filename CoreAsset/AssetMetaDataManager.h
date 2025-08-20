#pragma once

#include "CoreAsset/AssetMetaDataType.h"
#include "CoreAsset/AssetPtr.h"
#include "CoreAsset/AssetType.h"
#include <unordered_map>

namespace CoreAsset
{

class CORE_ASSET_API AssetMetaDataManager
{
  public:
    static AssetMetaDataManager *GetInstance();
    ~AssetMetaDataManager();

    bool Register(const AssetMetaData &assetMetaData);
    void UnRegister(AssetID id);

    AssetMetaData *GetMetaData(AssetID id) const;

  private:
    AssetMetaDataManager();

  private:
    // 타입별로 테이블

    using AssetMetaTable = std::unordered_map<AssetID, AssetMetaData *>;
    AssetMetaTable mAssetMetaDataTable;
};

} // namespace CoreAsset