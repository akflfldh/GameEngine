#pragma once



#include"D3DGpuResource.h"

#include<unordered_map>
#include"GpuResourceDllMacro.h"
namespace D3DGRM
{

	
	class GPURESOURCE_MANAGER_API D3DGpuTexture:public D3DGpuResource
	{


	public:
		D3DGpuTexture(Microsoft::WRL::ComPtr<ID3D12Resource> resource);
		virtual ~D3DGpuTexture();


	private:
		

	private:



	};


}
