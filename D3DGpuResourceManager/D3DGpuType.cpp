#include "D3DGpuType.h"
#include "D3DGpuResourceManager/D3DGpuType.h"
#include "D3DGpuType.h"

DXGI_FORMAT D3DGRM::ConvertToDxgiFormat(GRM::ETextureFormat format)
{
    switch (format)
    {
    case GRM::ETextureFormat::eR8_UNORM:
        return DXGI_FORMAT_R8_UNORM;
    case GRM::ETextureFormat::eR8G8_UNORM:
        return DXGI_FORMAT_R8G8_UNORM;
    case GRM::ETextureFormat::eR8G8B8A8_UNORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case GRM::ETextureFormat::eR8G8B8A8_SNORM:
        return DXGI_FORMAT_R8G8B8A8_SNORM;
    case GRM::ETextureFormat::eR8G8B8A8_UINT:
        return DXGI_FORMAT_R8G8B8A8_UINT;
    case GRM::ETextureFormat::eR8G8B8A8_SINT:
        return DXGI_FORMAT_R8G8B8A8_SINT;
    case GRM::ETextureFormat::eB8G8R8A8_UNORM:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    case GRM::ETextureFormat::eR8G8B8A8_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    case GRM::ETextureFormat::eB8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    case GRM::ETextureFormat::eB8G8R8X8_UNORM:
        return DXGI_FORMAT_B8G8R8X8_UNORM;
    case GRM::ETextureFormat::eB8G8R8X8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

    case GRM::ETextureFormat::eR16_FLOAT:
        return DXGI_FORMAT_R16_FLOAT;
    case GRM::ETextureFormat::eR16G16_FLOAT:
        return DXGI_FORMAT_R16G16_FLOAT;
    case GRM::ETextureFormat::eR16G16B16A16_FLOAT:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case GRM::ETextureFormat::eR16_UNORM:
        return DXGI_FORMAT_R16_UNORM;
    case GRM::ETextureFormat::eR16G16_UNORM:
        return DXGI_FORMAT_R16G16_UNORM;
    case GRM::ETextureFormat::eR16G16B16A16_UNORM:
        return DXGI_FORMAT_R16G16B16A16_UNORM;

    case GRM::ETextureFormat::eR32_FLOAT:
        return DXGI_FORMAT_R32_FLOAT;
    case GRM::ETextureFormat::eR32G32_FLOAT:
        return DXGI_FORMAT_R32G32_FLOAT;
    case GRM::ETextureFormat::eR32G32B32A32_FLOAT:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case GRM::ETextureFormat::eR32_UINT:
        return DXGI_FORMAT_R32_UINT;
    case GRM::ETextureFormat::eR32G32_UINT:
        return DXGI_FORMAT_R32G32_UINT;
    case GRM::ETextureFormat::eR32G32B32A32_UINT:
        return DXGI_FORMAT_R32G32B32A32_UINT;

    case GRM::ETextureFormat::eD32_FLOAT:
        return DXGI_FORMAT_D32_FLOAT;
    case GRM::ETextureFormat::eD24_UNORM_S8_UINT:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    case GRM::ETextureFormat::eD16_UNORM:
        return DXGI_FORMAT_D16_UNORM;

    case GRM::ETextureFormat::eBC1_UNORM:
        return DXGI_FORMAT_BC1_UNORM;
    case GRM::ETextureFormat::eBC2_UNORM:
        return DXGI_FORMAT_BC2_UNORM;
    case GRM::ETextureFormat::eBC3_UNORM:
        return DXGI_FORMAT_BC3_UNORM;
    case GRM::ETextureFormat::eBC4_UNORM:
        return DXGI_FORMAT_BC4_UNORM;
    case GRM::ETextureFormat::eBC5_UNORM:
        return DXGI_FORMAT_BC5_UNORM;
    case GRM::ETextureFormat::eBC7_UNORM:
        return DXGI_FORMAT_BC7_UNORM;
    case GRM::ETextureFormat::eBC1_UNORM_SRGB:
        return DXGI_FORMAT_BC1_UNORM_SRGB;
    case GRM::ETextureFormat::eBC2_UNORM_SRGB:
        return DXGI_FORMAT_BC2_UNORM_SRGB;
    case GRM::ETextureFormat::eBC3_UNORM_SRGB:
        return DXGI_FORMAT_BC3_UNORM_SRGB;
    case GRM::ETextureFormat::eBC7_UNORM_SRGB:
        return DXGI_FORMAT_BC7_UNORM_SRGB;

    default:
        return DXGI_FORMAT_UNKNOWN; // 알 수 없는 포맷은 DXGI_FORMAT_UNKNOWN으로 매핑
    }
}

D3D12_RESOURCE_DIMENSION D3DGRM::ConvertToResourceDimension(GRM::ETextureType type)
{

    switch (type)
    {
    case GRM::ETextureType::eTexture2D:
        return D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    default:

        return D3D12_RESOURCE_DIMENSION_UNKNOWN;
    }
}

D3D12_RESOURCE_FLAGS D3DGRM::ConvertToResourceFlags(GRM::ETextureUsage usage)
{
    switch (usage)
    {
    case GRM::ETextureUsage::eNone:
        return D3D12_RESOURCE_FLAG_NONE;
    case GRM::ETextureUsage::eRenderTarget:
        return D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    case GRM::ETextureUsage::eDepthStencil:
        return D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    case GRM::ETextureUsage::eRenderTargetShaderResource:
        return D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    default:
        return D3D12_RESOURCE_FLAG_NONE;
    }
}

D3D12_RTV_DIMENSION D3DGRM::ConvertToRTVDimension(GRM::ETextureType type)
{

    switch (type)
    {
    case GRM::ETextureType::eTexture2D:
        return D3D12_RTV_DIMENSION_TEXTURE2D;

    default:
        return D3D12_RTV_DIMENSION_UNKNOWN;
    }
}

D3D12_SRV_DIMENSION D3DGRM::ConvertToSRVDimension(GRM::ETextureType type)
{

    switch (type)
    {
    case GRM::ETextureType::eTexture2D:
        return D3D12_SRV_DIMENSION_TEXTURE2D;

    default:
        return D3D12_SRV_DIMENSION_UNKNOWN;
    }
}

D3D12_DSV_DIMENSION D3DGRM::ConvertToDSVDimension(GRM::ETextureType type)
{

    switch (type)
    {
    case GRM::ETextureType::eTexture2D:
        return D3D12_DSV_DIMENSION_TEXTURE2D;

    default:
        return D3D12_DSV_DIMENSION_UNKNOWN;
    }
}

GPURESOURCE_MANAGER_API D3D12_FILTER D3DGRM::ConvertToSamplerFilter(GRM::ESamplerFilterMode filter)
{

    switch (filter)
    {
    case GRM::ESamplerFilterMode::eMIN_MAG_MIP_LINEAR:
        return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    case GRM::ESamplerFilterMode::eMIN_MAG_MIP_POINT:
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    }
}

GPURESOURCE_MANAGER_API D3D12_TEXTURE_ADDRESS_MODE
D3DGRM::ConvertToSamplerAddressMode(GRM::ESamplerAddressMode addressMode)
{
    switch (addressMode)
    {
    case GRM::ESamplerAddressMode::eWrap:
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    case GRM::ESamplerAddressMode::eClamp:
        return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    }
}

GPURESOURCE_MANAGER_API D3D12_RESOURCE_STATES D3DGRM::ConvertToD3DResourceState(EResourceState resourceState)
{
    {

        switch (resourceState)
        {
        case EResourceState::eNone:
            return D3D12_RESOURCE_STATE_COMMON;
        case EResourceState::eRenderTarget:
            return D3D12_RESOURCE_STATE_RENDER_TARGET;
        case EResourceState::eGenericRead:
            return D3D12_RESOURCE_STATE_GENERIC_READ;
        case EResourceState::ePresent:
            return D3D12_RESOURCE_STATE_PRESENT;
        case EResourceState::eReadDepthStencil:
            return D3D12_RESOURCE_STATE_DEPTH_READ;
        case EResourceState::eWriteDepthStencil:
            return D3D12_RESOURCE_STATE_DEPTH_WRITE;
        case EResourceState::eCopySource:
            return D3D12_RESOURCE_STATE_COPY_SOURCE;
        case EResourceState::eCopyDest:
            return D3D12_RESOURCE_STATE_COPY_DEST;
        default:
            return D3D12_RESOURCE_STATE_COMMON;
        }
    }
}

GPURESOURCE_MANAGER_API EResourceState D3DGRM::ConvertFromD3D12ResourceState(D3D12_RESOURCE_STATES resourceState)
{

    switch (resourceState)
    {
        //   case D3D12_RESOURCE_STATE_COMMON: // 또는 D3D12_RESOURCE_STATE_PRESENT (0번 비트 공유 시)
        //       return EResourceState::eNone;

    case D3D12_RESOURCE_STATE_RENDER_TARGET:
        return EResourceState::eRenderTarget;

    case D3D12_RESOURCE_STATE_GENERIC_READ:
        return EResourceState::eGenericRead;

    case D3D12_RESOURCE_STATE_PRESENT:
        return EResourceState::ePresent;

    case D3D12_RESOURCE_STATE_DEPTH_READ:
        return EResourceState::eReadDepthStencil;

    case D3D12_RESOURCE_STATE_DEPTH_WRITE:
        return EResourceState::eWriteDepthStencil;

    case D3D12_RESOURCE_STATE_COPY_SOURCE:
        return EResourceState::eCopySource;

    case D3D12_RESOURCE_STATE_COPY_DEST:
        return EResourceState::eCopyDest;

    default:
        // 매칭되는 상태가 없거나 복합 상태일 경우 기본값 반환
        return EResourceState::eNone;
    }
}