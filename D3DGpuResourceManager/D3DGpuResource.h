#pragma once

#include"IGpuResource.h"
#include<d3d12.h>
#include<wrl.h>
#include<unordered_map>
#include"D3DGpuType.h"
#include"GpuResourceDllMacro.h"
namespace D3DGRM
{


	class GPURESOURCE_MANAGER_API D3DGpuResource:public GRM::IGpuResource
	{
		friend class D3DGpuResourceManager;
	public:
		~D3DGpuResource();

		Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const;
		bool GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType type, D3DDescriptorHandle& descriptorHandle);



		void SetResourceState(D3D12_RESOURCE_STATES state);
		D3D12_RESOURCE_STATES GetResourceState() const;

	protected:
		D3DGpuResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource);

		void SetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType type, const D3DDescriptorHandle descriptorHandle);

	private:
		void RemoveDescriptorHandle(D3DGRM::ED3DResourceDescriptorType type);
		void RemoveDescriptorHandleAll();

		Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
		std::unordered_map<D3DGRM::ED3DResourceDescriptorType, D3DDescriptorHandle> mDescriptorHandleTable;


		//상태를 추적할수있도록해야한다.
		D3D12_RESOURCE_STATES mResourceState;

	};


}