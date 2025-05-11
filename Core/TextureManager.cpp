#include "ResourceManager/TextureManager/TextureManager.h"

#include"Asset/Texture/RenderTargetTexture.h"

namespace Quad
{
	

	TextureManager::TextureManager()
	{

	}

	void TextureManager::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, DescriptorHeapManagerMaster* descriptorHeapManagerMaster, GraphicCommand* commandObject)
	{

		mTextureFactory.Initialize(device, descriptorHeapManagerMaster, commandObject);
	
	}

	Texture* TextureManager::CreateTexture(const std::string& name)
	{
		auto instance = GetInstance();
		if (instance->mNameTable.GetID(name) != 0)
			return nullptr;

		
		Texture * texture =	 instance->mTextureFactory.CreateTexture();
		texture->SetName(name);

		instance->mIDTable.Register(texture);
		instance->mNameTable.Register(name, texture->GetUniqueID());

		return texture;
	}

	Texture* TextureManager::CreateTextureFromFile(const std::string& name, unsigned long long id)
	{
		auto instance = GetInstance();
		if (instance->mIDTable.GetElement(id) != nullptr)
			return nullptr;


		Texture* texture = instance->mTextureFactory.CreateTexture();
		texture->SetName(name);

		instance->mIDTable.RegisterFromFile(texture,id);
		instance->mNameTable.Register(name, texture->GetUniqueID());

		return texture;
	}

	RenderTargetTexture* TextureManager::CreateRenderTargetTexture(const std::string& name, int width, int height)
	{
		auto instance = GetInstance();
		if (instance->mNameTable.GetID(name) != 0)
			return nullptr;


		RenderTargetTexture* texture = instance->mTextureFactory.CreateRenderTargetTexture(width, height);

		texture->SetName(name);
		instance->mIDTable.Register(texture);
		instance->mNameTable.Register(name, texture->GetUniqueID());
		return texture;
	}

	Texture* TextureManager::CreateDepthStencilBuffer(const std::string& name, int width, int height)
	{
		auto instance = GetInstance();
		if (instance->mNameTable.GetID(name) != 0)
			return nullptr;

		Texture* texture = instance->mTextureFactory.CreateDepthStencilBuffer(width, height);
		texture->SetName(name);

		instance->mIDTable.Register(texture);
		instance->mNameTable.Register(name, texture->GetUniqueID());

		return texture;
	}


	Texture* TextureManager::GetTexture(const std::string& name)
	{
		auto instance = GetInstance();

		unsigned long long id = instance->mNameTable.GetID(name);
		if (id == 0)
			return nullptr;

		return GetTexture(id);

	}

	Texture* TextureManager::GetTexture(unsigned long long id) 
	{
		auto instance = GetInstance();

		if (id == 0)
			return nullptr;

		return instance->mIDTable.GetElement(id);
	}

	

	void TextureManager::KillTexture(Texture* texture)
	{
		auto instance = GetInstance();
		instance->mDeadTable.push_back(texture);


		instance->mIDTable.UnRegister(texture->GetUniqueID());
		instance->mNameTable.UnRegister(texture->GetName());
		instance->mTextureFactory.ReleaseTexture(texture);



	}

	void TextureManager::KillTexture(const std::string& name)
	{
		auto instance = GetInstance();
		unsigned long long id = instance->mNameTable.GetID(name);
		Texture* texture = instance->mIDTable.GetElement(id);
		KillTexture(texture);

	}

	void TextureManager::RemoveDeadTexture()
	{
		/*auto instance = GetInstance();
		for (auto& texture : instance->mDeadTable)
		{

			instance->mIDTable.UnRegister(texture->GetUniqueID());
			instance->mNameTable.UnRegister(texture->GetName());
			instance->mTextureFactory.ReleaseTexture(texture);
		}

		instance->mDeadTable.resize(0);*/
	}



	

}