#include "CoreAsset/Asset.h"
#include "CoreAsset/GlobalAssetRegistrySystem.h"
#include <BinaryReaderWriter/BinaryWriter.h>

CoreAsset::Asset::~Asset() {}

const std::string &CoreAsset::Asset::GetName() const
{
    return mName;
}

void CoreAsset::Asset::SetTag(const std::string &tag)
{
    mTag = tag;
}

const std::string &CoreAsset::Asset::GetTag() const
{
    return mTag;
    // TODO: 여기에 return 문을 삽입합니다.
}

void CoreAsset::Asset::SetDirty()
{

    if (mDirtyFlag == true)
        return;

    GlobalAssetRegistrySystem *assetRegistrySystem = GlobalAssetRegistrySystem::GetInstance();
    assetRegistrySystem->AddDirtyAsset(this);

    mDirtyFlag = true;
}

void CoreAsset::Asset::ClearDirty()
{
    mDirtyFlag = false;
}

bool CoreAsset::Asset::GetDirty() const
{
    return mDirtyFlag;
}

void CoreAsset::Asset::Serialize(QuadRW::BinaryWriter &writer) const
{
    writer.Write((uint32_t)mType);

    writer.Write(mID);

    writer.Write(mName);
}