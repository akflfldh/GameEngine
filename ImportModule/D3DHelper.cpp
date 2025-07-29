#include "D3DHelper.h"

GRM::ETextureType ConvertTextureType(DirectX::TEX_DIMENSION dimension)
{

    switch (dimension)
    {

    case DirectX::TEX_DIMENSION_TEXTURE1D:

        return GRM::ETextureType::eTexture1D;


    case DirectX::TEX_DIMENSION_TEXTURE2D:

        return GRM::ETextureType::eTexture2D;

    case DirectX::TEX_DIMENSION_TEXTURE3D:

        return GRM::ETextureType::eTexture3D;

    }

}

GRM::ETextureFormat ConvertToDxgiFormat(DXGI_FORMAT dxgiFormat)
{
    using enum GRM::ETextureFormat;

    switch (dxgiFormat)
    {
        // --- 8-bit formats ---
    case DXGI_FORMAT_R8_UNORM: return eR8_UNORM;
    case DXGI_FORMAT_R8G8_UNORM: return eR8G8_UNORM;
    case DXGI_FORMAT_R8G8B8A8_UNORM: return eR8G8B8A8_UNORM;
    case DXGI_FORMAT_R8G8B8A8_SNORM: return eR8G8B8A8_SNORM;
    case DXGI_FORMAT_R8G8B8A8_UINT:  return eR8G8B8A8_UINT;
    case DXGI_FORMAT_R8G8B8A8_SINT:  return eR8G8B8A8_SINT;
    case  DXGI_FORMAT_B8G8R8A8_UNORM: return eB8G8R8A8_UNORM;

        // --- 16-bit formats ---
    case DXGI_FORMAT_R16_FLOAT: return eR16_FLOAT;
    case DXGI_FORMAT_R16G16_FLOAT: return eR16G16_FLOAT;
    case DXGI_FORMAT_R16G16B16A16_FLOAT: return eR16G16B16A16_FLOAT;
    case DXGI_FORMAT_R16_UNORM: return eR16_UNORM;
    case DXGI_FORMAT_R16G16_UNORM: return eR16G16_UNORM;
    case DXGI_FORMAT_R16G16B16A16_UNORM: return eR16G16B16A16_UNORM;

        // --- 32-bit formats ---
    case DXGI_FORMAT_R32_FLOAT: return eR32_FLOAT;
    case DXGI_FORMAT_R32G32_FLOAT: return eR32G32_FLOAT;
    case DXGI_FORMAT_R32G32B32_FLOAT: return eR32G32B32_FLOAT;
    case DXGI_FORMAT_R32G32B32A32_FLOAT: return eR32G32B32A32_FLOAT;
    case DXGI_FORMAT_R32_UINT: return eR32_UINT;
    case DXGI_FORMAT_R32G32_UINT: return eR32G32_UINT;
    case DXGI_FORMAT_R32G32B32_UINT: return eR32G32B32_UINT;
    case DXGI_FORMAT_R32G32B32A32_UINT: return eR32G32B32A32_UINT;

        // --- Depth/Stencil formats ---
    case DXGI_FORMAT_D32_FLOAT: return eD32_FLOAT;
    case DXGI_FORMAT_D24_UNORM_S8_UINT: return eD24_UNORM_S8_UINT;
    case DXGI_FORMAT_D16_UNORM: return eD16_UNORM;

        // --- Compressed formats ---
    case DXGI_FORMAT_BC1_UNORM: return eBC1_UNORM;
    case DXGI_FORMAT_BC2_UNORM: return eBC2_UNORM;
    case DXGI_FORMAT_BC3_UNORM: return eBC3_UNORM;
    case DXGI_FORMAT_BC4_UNORM: return eBC4_UNORM;
    case DXGI_FORMAT_BC5_UNORM: return eBC5_UNORM;
    case DXGI_FORMAT_BC7_UNORM: return eBC7_UNORM;

    default:
    
        return eUnknown;
    

    }
}