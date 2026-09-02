#pragma once

#include "CoreAsset/Asset.h"
#include "CoreAsset/AssetType.h"
#include "CoreAsset/CoreAssetDLLMacro.h"

class Arch;

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
    AssetID GetAssetID() const;

    template <typename T> T *As() const
    {
        static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset Class");

        Asset *pAsset = Get();
        if (pAsset == nullptr)
            return nullptr;

        EAssetType type = T::GetAssetType();

        return pAsset->GetType() == type ? static_cast<T *>(pAsset) : nullptr;
    }

    void SetAsset(Asset *asset);
    void SetAsset(AssetID assetID);

  private:
    mutable AssetID mID;
};

CORE_ASSET_API Arch &operator<<(Arch &arch, AssetPtr &ptr);

} // namespace CoreAsset