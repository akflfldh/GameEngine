#pragma once

#ifdef D3DX

#include "D3DGpuResourceManager/GpuResourceDllMacro.h"
#include <CommonHeader/GpuTypes.h>
#include <D3DGpuResourceManager/GpuResourceTypes.h>
#include <d3d12.h>

namespace D3DGRM
{
struct D3DDescriptorHandle
{
    D3D12_CPU_DESCRIPTOR_HANDLE mCpuDescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE mGpuDescriptorHandle;
};

GPURESOURCE_MANAGER_API DXGI_FORMAT ConvertToDxgiFormat(GRM::ETextureFormat format);

GPURESOURCE_MANAGER_API D3D12_RESOURCE_DIMENSION ConvertToResourceDimension(GRM::ETextureType type);

GPURESOURCE_MANAGER_API D3D12_RESOURCE_FLAGS ConvertToResourceFlags(GRM::ETextureUsage usage);

GPURESOURCE_MANAGER_API D3D12_RTV_DIMENSION ConvertToRTVDimension(GRM::ETextureType type);

GPURESOURCE_MANAGER_API D3D12_SRV_DIMENSION ConvertToSRVDimension(GRM::ETextureType type);

GPURESOURCE_MANAGER_API D3D12_DSV_DIMENSION ConvertToDSVDimension(GRM::ETextureType type);

GPURESOURCE_MANAGER_API D3D12_FILTER ConvertToSamplerFilter(GRM::ESamplerFilterMode filter);

GPURESOURCE_MANAGER_API D3D12_TEXTURE_ADDRESS_MODE ConvertToSamplerAddressMode(GRM::ESamplerAddressMode addressMode);

enum class ED3DResourceDescriptorType
{
    eCBV = 0,
    eSRV,
    eUAV,
    eDSV,
    eRTV,
    eSMP
};

GPURESOURCE_MANAGER_API D3D12_RESOURCE_STATES ConvertToD3DResourceState(EResourceState resourceState);

GPURESOURCE_MANAGER_API EResourceState ConvertFromD3D12ResourceState(D3D12_RESOURCE_STATES resourceState);
} // namespace D3DGRM

#endif