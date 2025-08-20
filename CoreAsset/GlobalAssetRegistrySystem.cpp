#include "CoreAsset/GlobalAssetRegistrySystem.h"
#include "CoreAsset/Asset.h"

CoreAsset::GlobalAssetRegistrySystem *CoreAsset::GlobalAssetRegistrySystem::GetInstance()
{
    static GlobalAssetRegistrySystem instance;
    return &instance;
}
CoreAsset::GlobalAssetRegistrySystem::GlobalAssetRegistrySystem() {}

CoreAsset::GlobalAssetRegistrySystem::~GlobalAssetRegistrySystem() {}

CoreAsset::Asset *CoreAsset::GlobalAssetRegistrySystem::GetAsset(AssetID id) const
{
    return mAssetTable.GetAsset(id);
}
CoreAsset::Asset *CoreAsset::GlobalAssetRegistrySystem::GetAsset(const std::string &name) const
{

    return mAssetTable.GetAsset(name);
}

CoreAsset::AssetID CoreAsset::GlobalAssetRegistrySystem::GetNextAssetID()
{

    return mAssetIDGenerator.GetNewAssetID();
}

bool CoreAsset::GlobalAssetRegistrySystem::RegisterAsset(Asset *asset, const std::string &name)
{

    if (mAssetTable.HasName(name) == true)
        return false;

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

const std::vector<CoreAsset::AssetPtr<CoreAsset::Asset>> &CoreAsset::GlobalAssetRegistrySystem::GetDirtyAssetList()
    const
{
    return mDirtyAssetList;
}

void CoreAsset::GlobalAssetRegistrySystem::SetNextAssetID(AssetID id)
{

    mAssetIDGenerator.SetNextAssetID(id);
}

CoreAsset::AssetID CoreAsset::GlobalAssetRegistrySystem::PeekNextAssetID() const
{
    return mAssetIDGenerator.PeekNextAssetID();
}