#pragma once


#include"SerializedAssetType.h"
#include<memory>
#include"CoreAssetDLLMacro.h"
namespace QuadRW { class BinaryReader; }


namespace CoreAsset
{
	class TextureLoader;
	class AssetMetaData;
	class CORE_ASSET_API AssetLoader
	{
	public:
		static AssetLoader* GetInstance();
		~AssetLoader();

		std::unique_ptr<SerializedAsset>LoadAsset(const std::string& filePath, AssetMetaData & oAssetMetaData);
		std::unique_ptr<SerializedAssetRawData> LoadAssetRawData(const std::string& filePath, EAssetType assetType);


	private:
		AssetLoader();


	private:
		//loader
		TextureLoader* mTextureLoader;





	};
}
