#include "CoreAsset/GlobalAssetRegistrySystem.h"
#include "CoreAsset/Asset.h"

CoreAsset::GlobalAssetRegistrySystem *CoreAsset::GlobalAssetRegistrySystem::GetInstance()
{
    static GlobalAssetRegistrySystem instance;
    return &instance;
}
CoreAsset::GlobalAssetRegistrySystem::GlobalAssetRegistrySystem()
{

    mAssetIDGenerator.SetNextAssetID(mEngineAssetLimit + 1);
}

CoreAsset::GlobalAssetRegistrySystem::~GlobalAssetRegistrySystem() {}

CoreAsset::Asset *CoreAsset::GlobalAssetRegistrySystem::GetAsset(AssetID id) const
{
    return mAssetTable.GetAsset(id);
}
CoreAsset::Asset *CoreAsset::GlobalAssetRegistrySystem::GetAsset(const std::string &name) const
{

    return mAssetTable.GetAsset(name);
}

void CoreAsset::GlobalAssetRegistrySystem::GetAssetsByType(CoreAsset::EAssetType type,
                                                           std::vector<Asset *> &oAssetList) const
{

    return mAssetTable.GetAssetsByType(type, oAssetList);
}

CoreAsset::AssetID CoreAsset::GlobalAssetRegistrySystem::GetNextAssetID(bool bEngine)
{
    if (bEngine)
    {
        return mEngineAssetIDGenerator.GetNewAssetID();
    }

    return mAssetIDGenerator.GetNewAssetID();
}

bool CoreAsset::GlobalAssetRegistrySystem::RegisterAsset(Asset *asset, const std::string &name, bool bEngine)
{

    if (mAssetTable.HasName(name) == true)
        return false;

    if (asset->GetID() == NoneAssetID)
    {
        if (bEngine)
        {
            asset->mID = GetNextAssetID(true);
        }
        else
        {
            asset->mID = GetNextAssetID(false);
        }
    }

    return mAssetTable.SetAsset(name, asset->GetID(), asset);
}

bool CoreAsset::GlobalAssetRegistrySystem::HasName(const std::string &name) const
{

    return mAssetTable.HasName(name);
}

void CoreAsset::GlobalAssetRegistrySystem::AddDirtyAsset(Asset *asset)
{
    if (asset == nullptr)
        return;

    // 이미추가되었다
    if (asset->GetDirty() == true)
        return;

    mDirtyAssetList.push_back(asset);
}

const std::vector<CoreAsset::AssetPtr> &CoreAsset::GlobalAssetRegistrySystem::GetDirtyAssetList() const
{
    return mDirtyAssetList;
}

void CoreAsset::GlobalAssetRegistrySystem::SetNextAssetID(AssetID id)
{
    // 엔진의 경우는 필요없다
    mAssetIDGenerator.SetNextAssetID(id);
}

CoreAsset::AssetID CoreAsset::GlobalAssetRegistrySystem::PeekNextAssetID() const
{
    // 엔진의 경우 필요없다.
    return mAssetIDGenerator.PeekNextAssetID();
}

void CoreAsset::GlobalAssetRegistrySystem::ClearDirtyAssetList()
{
    for (auto pAsset : mDirtyAssetList)
    {
        Asset *asset = pAsset.Get();

        if (asset)
        {
            asset->ClearDirty();
        }
    }

    mDirtyAssetList.clear();
}
size_t CoreAsset::GlobalAssetRegistrySystem::GetAssetNum() const
{

    return mAssetTable.GetAssetNum();
}