#include "AssetMetaDataManager.h"

CoreAsset::AssetMetaDataManager* CoreAsset::AssetMetaDataManager::GetInstance()
{
	static AssetMetaDataManager instance;


	return &instance;

}

CoreAsset::AssetMetaDataManager::AssetMetaDataManager()
{
	mAssetMetaDataTable[EAssetType::eTexture];



}

CoreAsset::AssetMetaDataManager::~AssetMetaDataManager()
{




}

bool CoreAsset::AssetMetaDataManager::Register(const AssetMetaData& assetMetaData)
{




	switch (assetMetaData.mAssetType)
	{
	case EAssetType::eTexture:

	{

		if (mAssetMetaDataTable[assetMetaData.mAssetType].find(assetMetaData.mAssetID) 
			!= mAssetMetaDataTable[assetMetaData.mAssetType].end())
		{
			return false;
		}

		TextureMetaData* textureMetaData = new TextureMetaData;
		*textureMetaData = static_cast<const TextureMetaData&>(assetMetaData);

		mAssetMetaDataTable[assetMetaData.mAssetType][assetMetaData.mAssetID] = textureMetaData;
		 
	}



	}





	return true;
}

void CoreAsset::AssetMetaDataManager::UnRegister(EAssetType assetType, AssetID id)
{

	std::unordered_map<AssetID, AssetMetaData*>::iterator it =  mAssetMetaDataTable[assetType].find(id);
	
	if (it == mAssetMetaDataTable[assetType].end())
		return;

	mAssetMetaDataTable[assetType].erase(it);


}

CoreAsset::TextureMetaData* CoreAsset::AssetMetaDataManager::GetTextureMetaData(AssetID id) const
{

	TypeIDMetaTable::const_iterator typeTableIt = mAssetMetaDataTable.find(EAssetType::eTexture);

	std::unordered_map<AssetID, AssetMetaData*>::const_iterator it = typeTableIt->second.find(id);
	
	return it == typeTableIt->second.cend() ? nullptr : static_cast<TextureMetaData*>(it->second);

}
