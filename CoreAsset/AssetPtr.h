#pragma once

#include "CoreAsset/Asset.h"
#include "CoreAsset/AssetType.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
namespace CoreAsset
{
class Asset;

// 댕글링 포인터를 방지하기위한 asset포인터,
//  댕글링 포인터방지가 목적이기에 래퍼런스카운팅,포인터소멸시 자동 에셋해제 등의 기능은 없다.
class CORE_ASSET_API AssetPtr
{
  public:
    AssetPtr(Asset *asse);
    AssetPtr(AssetID = NoneAssetID);
    ~AssetPtr();

    AssetPtr(const AssetPtr &) = default;
    AssetPtr &operator=(const AssetPtr &) = default;

    AssetPtr(AssetPtr &&) = default;
    AssetPtr &operator=(AssetPtr &&) = default;

    Asset *Get() const;

  private:
    mutable AssetID mID;
};

} // namespace CoreAsset