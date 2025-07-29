


#ifdef D3DX
#include "TextureImporter.h"
#include<DirectXTex.h>
#include<Utility.h>
#include<memory>
#include<GpuTypes.h>
#include<D3DGpuType.h>
#include"D3DHelper.h"
#include<TextureManager.h>
#include<AssetMetaDataManager.h>
#include<Texture.h>

#pragma comment(lib,"Utility.lib")




Import::TextureImporter* Import::TextureImporter::GetInstance()
{
	static TextureImporter instance;

	return &instance;

}

Import::TextureImporter::TextureImporter()
	/*:mTextureManager(textureManager),mAssetMetaDataManager(CoreAsset::AssetMetaDataManager::GetInstance())*/
{
	

}

Import::TextureImporter::~TextureImporter()
{
}

bool Import::TextureImporter::Import(const std::string& filePath , GRM::TextureDesc & oTextureDesc) const
{

	
	//utility get extension 
	GRM::TextureDesc & textureDesc = oTextureDesc;


	const std::wstring& filePathW = CoreUtility::Utility::ConvertToWString(filePath, true);

	DirectX::ScratchImage d3dScratchImage;
	DirectX::TexMetadata texMetaData;

 	HRESULT result =DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &texMetaData, d3dScratchImage);

	if (FAILED(result))
	{
		//log
		return false;
	}

	//텍스처데이터를위한 메타데이터를 채운다
	GRM::ScratchImage& grmScratchImage = textureDesc.mScratchImage;
	uint8_t* d3dScratchImagePixels = d3dScratchImage.GetPixels();//픽셀데이터의 시작위치

	grmScratchImage.mSize = d3dScratchImage.GetPixelsSize();
	grmScratchImage.mMemory.resize(grmScratchImage.mSize);


	memcpy(grmScratchImage.mMemory.data(), d3dScratchImagePixels, grmScratchImage.mSize);


	grmScratchImage.mimagesNum = d3dScratchImage.GetImageCount();
	grmScratchImage.mMetadata.mWidth = texMetaData.width;
	grmScratchImage.mMetadata.mHeight = texMetaData.height;
	grmScratchImage.mMetadata.mDepth = texMetaData.depth;
	grmScratchImage.mMetadata.mArraySize = texMetaData.arraySize;
	grmScratchImage.mMetadata.mMipLevels = texMetaData.mipLevels;
	grmScratchImage.mMetadata.mDimension =ConvertTextureType(texMetaData.dimension);
	grmScratchImage.mMetadata.mFormat = ConvertToDxgiFormat(texMetaData.format);
	if (grmScratchImage.mMetadata.mFormat == GRM::ETextureFormat::eUnknown)
	{
		//log
		assert(0);
	}

	grmScratchImage.mMetadata.mMiscFlags = texMetaData.miscFlags;
	grmScratchImage.mMetadata.mMiscFlags2 = texMetaData.miscFlags2;

	grmScratchImage.mImages.resize(d3dScratchImage.GetImageCount());

	const DirectX::Image * d3dImages = d3dScratchImage.GetImages();


	
	for (int i = 0; i < d3dScratchImage.GetImageCount(); ++i)
	{
		uintptr_t ptrOffset = d3dImages[i].pixels- d3dScratchImagePixels;

		grmScratchImage.mImages[i].mPixels = &grmScratchImage.mMemory[ptrOffset];
		grmScratchImage.mImages[i].mWidth = d3dImages[i].width;
		grmScratchImage.mImages[i].mHeight = d3dImages[i].height;
		grmScratchImage.mImages[i].mRowPitch = d3dImages[i].rowPitch;
		grmScratchImage.mImages[i].mSlicePitch = d3dImages[i].slicePitch;
		grmScratchImage.mImages[i].mFormat = ConvertToDxgiFormat(d3dImages[i].format);
	
	}
	
	textureDesc.mTextureUsage = GRM::ETextureUsage::eShaderResource;
	//textureManager에게 ScratchImage 전달 


	//여기까지가 임포터 코어 부분이다. 분리할것

	return true;

















	//std::string fileName = CoreUtility::Utility::GetFileNameFromPath(filePath);
	//if (mTextureManager == nullptr)
	//{
	//	assert(0);
	//}



	//CoreAsset::Texture* texture  = mTextureManager->CreateTexture(textureDesc, fileName, logicalPath);

	//if (texture)
	//{
	//	
	//	ReigsterTextureMetaData(texture, filePath);
	//}



	//return texture;

}

//void Import::TextureImporter::ReigsterTextureMetaData(CoreAsset::Texture* texture, const std::string& filePath) const
//{
//	CoreAsset::TextureMetaData metaData;
//
//
//	metaData.mAssetID = texture->GetID();
//	metaData.mAssetName = texture->GetName();
//	metaData.mAssetType = texture->GetType();
//	metaData.mFilePath = filePath;
//
//	mAssetMetaDataManager->Register(metaData);
//
//}


#endif