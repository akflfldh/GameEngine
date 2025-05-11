#include "Buffer/ConstantBufferManager.h"
#include"Buffer/ConstantBuffer.h"

void Quad::ConstantBufferManager::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device)
{
	mDevice = device;
}

void Quad::ConstantBufferManager::Update()
{
	//매프레임마다 상수버퍼들의 count를 0으로 초기화


	for (auto it = mConstantBufferUnMap.begin(); it != mConstantBufferUnMap.end(); ++it)
	{
		it->second->mUploadCount = 0;
	}




}

Quad::ConstantBuffer* Quad::ConstantBufferManager::CreateConstantBuffer(const std::string& name, UINT elementSize, UINT elementNum)
{
	ConstantBuffer* constantBuffer = nullptr;
	if (GetConstantBuffer(name) == nullptr)
	{
		constantBuffer = new ConstantBuffer(mDevice, elementSize, elementNum);
		mConstantBufferUnMap.insert({ name,constantBuffer });
	}


	return constantBuffer;
}

Quad::ConstantBuffer* Quad::ConstantBufferManager::GetConstantBuffer(UINT dataSize)
{
	ConstantBufferManager* constantBufferInstance = GetInstance();
	//에니메이션을 위한 버퍼를 만들어나야겠다.
	int bufferIndex = dataSize / 256;
	std::string name;
	switch (bufferIndex)
	{
	case 0:
	{
		name = "default256";
	}
	break;
	case 1:
	{
		name ="default512";
	}
	break;
	case 2:
	{
		name = "default768";
	}
	break;

	case 3:
	{
		name = "default1024";
	}
	break;
	case 4:
	{
		name = "default1280";
	}
	break;
	case 5:
	{
		name = "default1536";
	}
	break;
	default:
	{
		if (bufferIndex <= 65)
			name = "default16640";
		else
		{
			MessageBox(nullptr, L"상수버퍼 큰게 필요", L"error", 0);
			assert(0);
		}
	}
	break;
	}
	std::unordered_map<std::string, ConstantBuffer*>::iterator it = constantBufferInstance->mConstantBufferUnMap.find(name);
	return it != constantBufferInstance->mConstantBufferUnMap.end() ? it->second : nullptr;

}

Quad::ConstantBuffer* Quad::ConstantBufferManager::GetConstantBuffer(const std::string & name)
{
	ConstantBufferManager* constantBufferInstance = GetInstance();

	std::unordered_map<std::string, ConstantBuffer*>::iterator it = constantBufferInstance->mConstantBufferUnMap.find(name);
	return it != constantBufferInstance->mConstantBufferUnMap.end() ? it->second : nullptr;

}
