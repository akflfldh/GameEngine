#include "TextureStorer.h"

#include"Texture.h"
#include"AssetMetaDataType.h"
#include"AssetCommon.h"


#include"SerializedAssetType.h"

#include<fstream>

CoreAsset::TextureStorer* CoreAsset::TextureStorer::GetInstance()
{
	static TextureStorer instance;
	return &instance;

}



CoreAsset::TextureStorer::TextureStorer()
{
}

CoreAsset::TextureStorer::~TextureStorer()
{
}

bool CoreAsset::TextureStorer::Store(CoreAsset::Asset* asset, CoreAsset::AssetMetaData* metaData, const std::string& filePath)
{

	Texture* texture = static_cast<Texture*>(asset);
	TextureMetaData* textureMetaData = static_cast<TextureMetaData*>(metaData);
	

	//binary 포맷
	if (texture == nullptr)
		return false;



	SerializedTexture serializedTexture;
	serializedTexture.mAssetID = asset->GetID();
	serializedTexture.mAssetType = asset->GetType();
	serializedTexture.mAssetName = asset->GetName();
	serializedTexture.mTextureProperties = texture->GetProperties();
	serializedTexture.mTextureUsage = texture->GetRawData()->mTextureUsage;

	//현재경로설정
	
	mWriter.StartWrite();
	serializedTexture.Serialize(mWriter);
	
	//metaData기록여부
	bool metaDataFlag = true;
	if (metaData == nullptr)
		metaDataFlag = false;	
	mWriter.Write(metaDataFlag);


	if (metaData)
		metaData->Serialize(mWriter);



	bool ret =mWriter.Close(filePath);
	if (ret == false)
	{
		//log
		return false;
	}
	return true;
}

bool CoreAsset::TextureStorer::StoreTextureRaw(const std::string & texRawPath ,const GRM::TextureDesc & textureDesc)
{

	mWriter.StartWrite();
	
	SerializedTextureRawData serializedRawData;
	serializedRawData.mTextureRawData = textureDesc;
	serializedRawData.Serialize(mWriter);
//	mWriter.Write((unsigned int)textureDesc.mTextureUsage);
	return mWriter.Close(texRawPath);
}
