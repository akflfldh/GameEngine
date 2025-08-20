#include "CoreAsset/AssetIDGenerator.h"

CoreAsset::AssetIDGenerator::AssetIDGenerator() : mNextNewAssetID(1) {}

CoreAsset::AssetIDGenerator::~AssetIDGenerator() {}

CoreAsset::AssetID CoreAsset::AssetIDGenerator::GetNewAssetID()
{

    if (mFreeIDVector.empty())
        return mNextNewAssetID++;

    AssetID nextAssetID = mFreeIDVector.back();
    mFreeIDVector.pop_back();

    return nextAssetID;
}

CoreAsset::AssetID CoreAsset::AssetIDGenerator::PeekNextAssetID() const
{

    if (mFreeIDVector.empty())
        return mNextNewAssetID;

    return mFreeIDVector.back();
}

void CoreAsset::AssetIDGenerator::ReleaseAssetID(AssetID id)
{

    mFreeIDVector.push_back(id);
}

void CoreAsset::AssetIDGenerator::SetNextAssetID(AssetID id)
{
    mNextNewAssetID = id;
}