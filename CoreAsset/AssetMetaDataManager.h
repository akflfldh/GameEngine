#pragma once

#include"AssetType.h"
#include<unordered_map>
#include"AssetPtr.h"
#include"AssetMetaDataType.h"


namespace CoreAsset
{

	class CORE_ASSET_API AssetMetaDataManager
	{
	public:
		static AssetMetaDataManager* GetInstance();
		~AssetMetaDataManager();


		bool Register(const AssetMetaData& assetMetaData);
		void UnRegister(EAssetType assetType, AssetID id);


		TextureMetaData* GetTextureMetaData(AssetID id) const;


	private:
		AssetMetaDataManager();

	private:
		//타입별로 테이블

		using TypeIDMetaTable = std::unordered_map < EAssetType, std::unordered_map<AssetID, AssetMetaData*>>;
		TypeIDMetaTable mAssetMetaDataTable;

	};


}