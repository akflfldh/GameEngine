#pragma once

#include"IGpuResource.h"
#include<d3d12.h>
#include<wrl.h>
#include"D3DGpuType.h"

namespace D3DGRM
{


	class D3DGpuResource:public GRM::IGpuResource
	{
		friend class D3DGpuResourceManager;
	public:
		~D3DGpuResource();

	

	protected:
		D3DGpuResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource);
		Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const;
		void SetDescriptorHandle(D3DDescriptorHandle handle);


	private:

		Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
		D3DDescriptorHandle mDescriptorHandle;

	};


}