#pragma once

#include"Predeclare.h"
#include"header.h"
#include"Singleton.h"

namespace Quad
{
	class ConstantBufferManager:public Singleton<ConstantBufferManager>
	{
	public:
		ConstantBufferManager() = default;
		~ConstantBufferManager() = default;

		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device);
		void Update();

		ConstantBuffer* CreateConstantBuffer(const std::string& name, UINT elementSize, UINT elementNum);
		static ConstantBuffer* GetConstantBuffer(UINT dataSize);
		static ConstantBuffer* GetConstantBuffer(const std::string & name);

	private:
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		std::unordered_map<std::string, ConstantBuffer*> mConstantBufferUnMap;


	};

}
