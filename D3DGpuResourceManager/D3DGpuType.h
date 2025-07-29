#pragma once

#include<d3d12.h>
#include"GpuTypes.h"
#include"GpuResourceDllMacro.h"
namespace D3DGRM
{
    struct D3DDescriptorHandle
    {
        D3D12_CPU_DESCRIPTOR_HANDLE mCpuDescriptorHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE mGpuDescriptorHandle;

    };


    GPURESOURCE_MANAGER_API  DXGI_FORMAT ConvertToDxgiFormat(GRM::ETextureFormat format) ;
  


    GPURESOURCE_MANAGER_API D3D12_RESOURCE_DIMENSION ConvertToResourceDimension(GRM::ETextureType type);



    GPURESOURCE_MANAGER_API D3D12_RESOURCE_FLAGS ConvertToResourceFlags(GRM::ETextureUsage usage);


    GPURESOURCE_MANAGER_API D3D12_RTV_DIMENSION ConvertToRTVDimension(GRM::ETextureType type);

    GPURESOURCE_MANAGER_API D3D12_SRV_DIMENSION ConvertToSRVDimension(GRM::ETextureType type);

    GPURESOURCE_MANAGER_API D3D12_DSV_DIMENSION ConvertToDSVDimension(GRM::ETextureType type);
       


    enum class ED3DResourceDescriptorType
    {
        eCBV = 0,
        eSRV,
        eUAV,
        eDSV,
        eRTV,
        eSMP
    };


}