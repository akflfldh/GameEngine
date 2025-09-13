#pragma once

#include "CoreAsset/AssetMetaDataType.h"
#include "CoreAsset/AssetPtr.h"
#include "CoreAsset/AssetType.h"
#include <unordered_map>

namespace CoreAsset
{
// 정확히 외부메타데이터를 유지한다.
// 내부 메타데이터(ex texture : width,height, miplevels등은 각 asset들이 직접 유지한다)
class CORE_ASSET_API AssetMetaDataManager
{
  public:
    static AssetMetaDataManager *GetInstance();
    ~AssetMetaDataManager();

    bool Register(const AssetMetaData &assetMetaData);
    bool Register(Asset *asset);
    void UnRegister(AssetID id);

    AssetMetaData *GetMetaData(AssetID id) const;
    AssetMetaData *GetMetaData(Asset *asset) const;

  private:
    AssetMetaDataManager();

  private:
    // 타입별로 테이블

    using AssetMetaTable = std::unordered_map<AssetID, AssetMetaData *>;
    AssetMetaTable mAssetMetaDataTable;
};

} // namespace CoreAsset