#pragma once

#include"D3DGpuResource.h"
#include<d3d12.h>
#include<wrl.h>
#include"GpuResourceDllMacro.h"
namespace D3DGRM
{

	class GPURESOURCE_MANAGER_API D3DGpuBuffer:public D3DGpuResource
	{
	public:
		D3DGpuBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> resource);
		virtual ~D3DGpuBuffer();

	private:
	


	private:
		


	};

}
