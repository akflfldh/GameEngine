#include "CoreAsset/AssetPtr.h"
#include "CoreAsset/Texture.h"
#include "CoreAsset/TextureManager.h"

#include "CoreAsset/Material.h"
#include "CoreAsset/MaterialManager.h"

#include "CoreAsset/GlobalAssetRegistrySystem.h"

template <typename T> class AssetManagerAccessor
{
  public:
    static_assert(sizeof(T) == 0, "AssetManagerAccessor<T> is not specialized for this type.");

  private:
};

template <> class AssetManagerAccessor<CoreAsset::Asset>
{
  public:
    static CoreAsset::GlobalAssetRegistrySystem *GetManager()
    {
        return CoreAsset::GlobalAssetRegistrySystem::GetInstance();
    }

  private:
};

template <> class AssetManagerAccessor<CoreAsset::Texture>
{
  public:
    static CoreAsset::TextureManager *GetManager()
    {
        return CoreAsset::TextureManager::GetInstance();
    }

  private:
};

template <> class AssetManagerAccessor<CoreAsset::Material>
{
  public:
    static CoreAsset::MaterialManager *GetManager()
    {
        return CoreAsset::MaterialManager::GetInstance();
    }

  private:
};

template <typename T> T *CoreAsset::AssetPtr<T>::Get() const
{
    // AssetManagerAccessor ->get(id); //항상 look up

    auto manager = AssetManagerAccessor<T>::GetManager();

    if (mID == NoneAssetID)
        return nullptr;

    T *asset = manager->GetAsset(mID);
    return asset;
}

template <typename T> inline CoreAsset::AssetPtr<T>::AssetPtr(T *asset)
{
    if (asset != nullptr)
        mID = asset->GetID();
    else
        mID = NoneAssetID;
}

template <typename T> inline CoreAsset::AssetPtr<T>::~AssetPtr() {}

template <typename T> CoreAsset::AssetPtr<T>::AssetPtr(const AssetPtr &rhs)
{
    mID = rhs.mID;
}

template <typename T> CoreAsset::AssetPtr<T> &CoreAsset::AssetPtr<T>::operator=(const AssetPtr &rhs)
{

    mID = rhs.mID;

    return *this;
}

template <typename T> CoreAsset::AssetPtr<T>::AssetPtr(AssetPtr &&rhs)
{
    mID = rhs.mID;
    rhs.mID = NoneAssetID;
}

template <typename T> CoreAsset::AssetPtr<T> &CoreAsset::AssetPtr<T>::operator=(AssetPtr &&rhs)
{

    mID = rhs.mID;
    rhs.mID = NoneAssetID;

    return *this;
}

// template<>
// class AssetManagerAcessor<CoreAsset::Material>
//{
// public:
//	static CoreAsset::MaterialManager* GetManager() { return CoreAsset::MaterialManager::GetInstance(); }
//
// private:
// };

// template class AssetManagerAccessor<CoreAsset::Texture>;
template class CoreAsset::AssetPtr<CoreAsset::Asset>;
template class CoreAsset::AssetPtr<CoreAsset::Texture>;
template class CoreAsset::AssetPtr<CoreAsset::Material>;

// template class CoreAsset::AssetPtr<CoreAsset::Material>;