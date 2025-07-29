#pragma once


#include<string>
#include<memory>
#include"SerializedAssetType.h"

#include"CoreAssetDLLMacro.h"
namespace CoreAsset
{ 

	struct SerializedTexture;
	struct SerializedTextureRawData;
	class AssetMetaData;

	class CORE_ASSET_API TextureLoader
	{
	public:
		static TextureLoader* GetInstance();
		~TextureLoader();

		std::unique_ptr<SerializedTexture> LoadAssetFile(const std::string& filePath, CoreAsset::AssetMetaData& oMetaData);
		std::unique_ptr<SerializedTextureRawData> LoadRawFile(const std::string& filePath);
		

	private:
		TextureLoader();


	};







}

