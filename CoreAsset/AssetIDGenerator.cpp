#include "AssetIDGenerator.h"

CoreAsset::AssetIDGenerator::AssetIDGenerator()
	:mNextNewAssetID(1)
{
}

CoreAsset::AssetIDGenerator::~AssetIDGenerator()
{
}

CoreAsset::AssetID CoreAsset::AssetIDGenerator::GetNewAssetID()
{
	

	if (mFreeIDVector.empty())
		return mNextNewAssetID++;



	AssetID nextAssetID = mFreeIDVector.back();
	mFreeIDVector.pop_back();

	return nextAssetID;
}

void CoreAsset::AssetIDGenerator::ReleaseAssetID(AssetID id)
{

	mFreeIDVector.push_back(id);


}
