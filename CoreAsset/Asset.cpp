#include "CoreAsset/Asset.h"
#include "CoreAsset/GlobalAssetRegistrySystem.h"
#include <BinaryReaderWriter/BinaryReader.h>
#include <BinaryReaderWriter/BinaryWriter.h>
#include <CoreBase/Arch.h>

CoreAsset::Asset::Asset(CoreAsset::EAssetType assetType, AssetID id)
    : mID(id), mName(""), mTag(""), mType(assetType), mLoadState(LoadState::Unloaded), mDirtyFlag(false), mIsEmpty(true)
{
}

void CoreAsset::AssetHeaderContext::Serialize(QuadRW::BinaryWriter &binaryWriter)
{

    binaryWriter.Write((uint64_t)mID);
    binaryWriter.Write((uint32_t)mType);
}
void CoreAsset::AssetHeaderContext::DeSerialize(QuadRW::BinaryReader &binaryReader)
{

    uint32_t type;
    binaryReader.Read(mID);
    binaryReader.Read(type);
    mType = (EAssetType)type;
}

CoreAsset::Asset::~Asset() {}

const FString &CoreAsset::Asset::GetName() const
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
void CoreAsset::Asset::Serialize(Arch &arch)
{
    arch << mID;

    if (arch.GetLoadingFlag())
    {
        uint32_t type;

        arch << type;

        mType = (EAssetType)type;
    }
    else
    {
        uint32_t type = (uint32_t)mType;
        arch << type;
    }

    arch << mName;
}

void CoreAsset::Asset::Serialize(QuadRW::BinaryWriter &writer) const
{

    /*   AssetHeaderContext headerContext;
       headerContext.mID = mID;
       headerContext.mType = mType;
       headerContext.Serialize(writer);

       writer.Write(mName);*/
}

bool CoreAsset::Asset::IsEmptyAsset() const
{

    return mIsEmpty;
}

void CoreAsset::Asset::SetName(const FString &name)
{

    mName = name;
}

void CoreAsset::Asset::SetEmptyAssetFlag(bool flag)
{

    mIsEmpty = flag;
}
