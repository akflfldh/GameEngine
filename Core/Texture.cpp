#include "Asset/Texture/Texture.h"

namespace Quad
{
	Texture::Texture(const std::string & name)
		:Asset(name,EAssetType::eTexture)
	{
	}


	void Texture::SetTextureResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource)
	{

		//if(mResource!=nullptr)
		//	mResource->Release();

		mResource = resource;
	}

	void Texture::SetViewIndex(ViewIndex viewIndex)
	{
		mViewIndex = viewIndex;
	}

	
	Microsoft::WRL::ComPtr<ID3D12Resource> Texture::GetResource() const
	{
		return mResource;
	}

	ViewIndex Texture::GetViewIndex() const
	{
		return mViewIndex;
	}

	void Texture::SetTextureFormat(ETextureFormat format)
	{
		mTextureFormat = format;


	}

	ETextureFormat Texture::GetTextureFormat() const
	{
		return mTextureFormat;
	}

	void Texture::SetTextureType(ETextureType type)
	{
		mTextureType = type;
	}

	ETextureType Texture::GetTextureType() const
	{
		return mTextureType;
	}

	void Texture::Serialize(const std::string& tag )
	{
	}

	void Texture::DeSerialize(const std::string& tag )
	{
	}

	bool Texture::CheckType(const ContentItem* source)
	{
		bool ret =	Asset::CheckType(source);

		if (!ret)
			return false;

		const Asset* asset = static_cast<const Asset*>(source);
		
		if (asset->GetAssetType() != Quad::EAssetType::eTexture)
			return false;

		return true;

	}

	bool Texture::InnerCopy(const ContentItem* source)
	{




		return true;
	}

}