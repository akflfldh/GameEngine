#include "EditorTextureImporter.h"
#include<assert.h>

#include<TextureManager.h>
#include<TextureImporter.h>
#include<Utility.h>
#include<AssetMetaDataType.h>
#include<Texture.h>
#include<AssetMetaDataManager.h>
Quad::EditorTextureImporter* Quad::EditorTextureImporter::mInstance = nullptr;

Quad::EditorTextureImporter* Quad::EditorTextureImporter::GetInstance()
{

	if (mInstance == nullptr)
	{
		//log
		assert(0);
	}


	return mInstance;

}

Quad::EditorTextureImporter::EditorTextureImporter(Import::TextureImporter* textureImporterCore, CoreAsset::TextureManager* textureManager,
	CoreAsset::AssetMetaDataManager* assetMetaDataManager)
{
	if (mInstance != nullptr  || textureImporterCore==nullptr  || textureManager ==nullptr )
	{
		assert(0);
	}


	mInstance = this;
	mTextureImporterCore = textureImporterCore;
	mTextureManager = textureManager;
	mAssetMetaDataManager = assetMetaDataManager;

}

Quad::EditorTextureImporter::~EditorTextureImporter()
{
}

CoreAsset::Texture* Quad::EditorTextureImporter::Import(const std::string& filePath, const std::string& logicalPath) const
{

	GRM::TextureDesc textureDesc;
	bool ret = mTextureImporterCore->Import(filePath, textureDesc);
	if (ret == false)
		return nullptr;


	std::string fileName = CoreUtility::Utility::GetFileNameFromPath(filePath);
	CoreAsset::Texture* texture  = mTextureManager->CreateTexture(textureDesc, fileName, logicalPath,true);
	

	if (texture)
	{
		ReigsterTextureMetaData(texture, filePath,logicalPath);
		texture->SetDirty();
	}

	return texture;

}

bool Quad::EditorTextureImporter::ReigsterTextureMetaData(CoreAsset::Texture* texture, const std::string& filePath,
	const std::string& logicalPath) const
{
	CoreAsset::TextureMetaData metaData;
	metaData.mAssetID = texture->GetID();
	metaData.mAssetName = texture->GetName();
	metaData.mAssetType = texture->GetType();
	metaData.mFilePath = filePath;
	metaData.mKeepRawDataFlag = true;

	return mAssetMetaDataManager->Register(metaData);

}
