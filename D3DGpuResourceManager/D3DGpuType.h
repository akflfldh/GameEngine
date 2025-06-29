#pragma once

#include<d3d12.h>
namespace D3DGRM
{
	struct D3DDescriptorHandle
	{
		D3D12_CPU_DESCRIPTOR_HANDLE mCpuDescriptorHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE mGpuDescriptorHandle;


	};


}