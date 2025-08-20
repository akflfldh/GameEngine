#pragma once

#include "CoreAsset/Asset.h"
#include "CoreAsset/AssetType.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
namespace CoreAsset
{

// 댕글링 포인터를 방지하기위한 asset포인터,
//  댕글링 포인터방지가 목적이기에 래퍼런스카운팅,포인터소멸시 자동 에셋해제 등의 기능은 없다.
template <typename T> class CORE_ASSET_API AssetPtr
{
  public:
    AssetPtr(T *asset = nullptr);
    ~AssetPtr();

    AssetPtr(const AssetPtr &);
    AssetPtr &operator=(const AssetPtr &);

    AssetPtr(AssetPtr &&);
    AssetPtr &operator=(AssetPtr &&);

    T *Get() const;

  private:
    AssetID mID;
};

} // namespace CoreAsset