#include "Storer.h"
#include"TextureStorer.h"

#include<AssetType.h>
#include<Texture.h>
#include<AssetMetaDataType.h>

QuadSR::Storer* QuadSR::Storer::GetInstance()
{
	static Storer instance;
	return &instance;

}

QuadSR::Storer::Storer()
	:mTextureStorer(TextureStorer::GetInstance())
{






}

QuadSR::Storer::~Storer()
{


}

bool QuadSR::Storer::StoreAsset(CoreAsset::Asset* asset, CoreAsset::AssetMetaData* assetMetaData , const std::string & path)
{
	if (asset == nullptr)
		return false;
	
	
	bool ret = false;
	switch (asset->GetType())
	{
	case CoreAsset::EAssetType::eTexture:
	{

		ret = mTextureStorer->Store(
			static_cast<CoreAsset::Texture*>(asset), 
			static_cast<CoreAsset::TextureMetaData*>(assetMetaData),
			path);

	}
		break;
	}




	return ret;
}
