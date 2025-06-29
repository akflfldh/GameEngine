#include "ResourceManager/TextureManager/TextureManager.h"

#include"Asset/Texture/RenderTargetTexture.h"

#include<sstream>

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

	Texture* TextureManager::CreateTexture(const std::string& name, int width, int height)
	{
		auto instance = GetInstance();
		if (instance->mNameTable.GetID(name) != 0)
			return nullptr;


		Texture* texture = instance->mTextureFactory.CreateTexture(width, height);

		texture->SetName(name);
		instance->mIDTable.Register(texture);
		instance->mNameTable.Register(name, texture->GetUniqueID());
		return texture;

	
	}

	Texture* TextureManager::CreateTextureRandomName(int width, int height)
	{

		unsigned long long digit = 0;
		std::string preName = "Texture";
		std::string name;
		auto instance = GetInstance();
		while (1)
		{
			name = preName + std::to_string(digit);

			if (instance->mNameTable.GetID(name) == 0)
			{
				break;
			}

			digit += 1;
		}





		return CreateTexture(name,width,height);
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

	RenderTargetTexture* TextureManager::CreateRenderTargetTextureRandomName(int width, int height)
	{
		unsigned long long digit = 0;
		std::string preName = "RenderTargetTexture";
		std::string name;
		auto instance = GetInstance();
		while (1)
		{
			name = preName + std::to_string(digit);

			if (instance->mNameTable.GetID(name) == 0)
			{
				break;
			}

			digit += 1;
		}

		return	CreateRenderTargetTexture(name, width, height);
	
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

	Texture* TextureManager::CreateDepthStencilBufferRandomName(int width, int height)
	{

		unsigned long long digit = 0;
		std::string preName = "DepthStencilBuffer";
		std::string name;
		auto instance = GetInstance();
		while (1)
		{
			name = preName + std::to_string(digit);

			if (instance->mNameTable.GetID(name) == 0)
			{
				break;
			}

			digit += 1;
		}





		return CreateDepthStencilBuffer(name,width,height);
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

	void TextureManager::CreateRenderTargetTextureResource(RenderTargetTexture* renderTargetTexture, unsigned int width, unsigned int height)
	{
		auto instance = GetInstance();
		instance->mTextureFactory.CreateRenderTargetTextureResource(renderTargetTexture, width, height);
		


	}

	void TextureManager::CreateDepthStencilBufferResource(Texture* depthStencilBuffer, unsigned int width, unsigned int height)
	{
		auto instance = GetInstance();
		instance->mTextureFactory.CreateDepthStencilBufferResource(depthStencilBuffer, width, height);

	}

	void TextureManager::ResizeTexture(Texture* texture, unsigned int width, unsigned int height)
	{
		if (texture == nullptr)
			return;


		auto instance = GetInstance();
		
		ReleaseTextureResource(texture);


		switch(texture->GetTextureType())
		{
		case ETextureType::eDefaultTexture:
		
			OutputDebugStringW(L"디폴트 텍스처 resizeTexture는 구현되지않음\n");
			assert(0);
			break;

		case ETextureType::eRenderTargetTexture:

			CreateRenderTargetTextureResource(static_cast<RenderTargetTexture*>(texture), width, height);

			break;

		case ETextureType::eDepthStencilBuffer:

			CreateDepthStencilBufferResource(texture, width, height);
			break;
		}




	}

	
	

	void TextureManager::ReleaseTextureResource(Texture* texture)
	{

		if (texture == nullptr)
			return;

		auto instance = GetInstance();

		instance->mTextureFactory.ReleaseTextureResource(texture);


	}

	

	void TextureManager::KillTexture(Texture* texture)
	{
		auto instance = GetInstance();
		instance->mDeadTable.push_back(texture);

		instance->mIDTable.UnRegister(texture->GetUniqueID());
		instance->mNameTable.UnRegister(texture->GetName());



		instance->mTextureFactory.ReleaseTexture(texture);



	}

	void TextureManager::KillTexture(unsigned long long id)
	{
		auto instance = GetInstance();
		Texture* texture = instance->mIDTable.GetElement(id);
		KillTexture(texture);
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