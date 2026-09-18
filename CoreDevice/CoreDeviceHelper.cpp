
#include <CoreDevice/CoreDeviceHelper.h>

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
    case DXGI_FORMAT_UNKNOWN:
        return eUnknown;

    case DXGI_FORMAT_R8_UNORM:
        return eR8_UNORM;
    case DXGI_FORMAT_R8G8_UNORM:
        return eR8G8_UNORM;
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return eR8G8B8A8_UNORM;
    case DXGI_FORMAT_R8G8B8A8_SNORM:
        return eR8G8B8A8_SNORM;
    case DXGI_FORMAT_R8G8B8A8_UINT:
        return eR8G8B8A8_UINT;
    case DXGI_FORMAT_R8G8B8A8_SINT:
        return eR8G8B8A8_SINT;
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        return eB8G8R8A8_UNORM;
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return eR8G8B8A8_UNORM_SRGB;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return eB8G8R8A8_UNORM_SRGB;
    case DXGI_FORMAT_B8G8R8X8_UNORM:
        return eB8G8R8X8_UNORM;
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return eB8G8R8X8_UNORM_SRGB;
    case DXGI_FORMAT_R8_TYPELESS:
        return eR8_TYPELESS;
    case DXGI_FORMAT_R8_UINT:
        return eR8_UINT;
    case DXGI_FORMAT_R8_SNORM:
        return eR8_SNORM;
    case DXGI_FORMAT_R8_SINT:
        return eR8_SINT;
    case DXGI_FORMAT_R8G8_TYPELESS:
        return eR8G8_TYPELESS;
    case DXGI_FORMAT_R8G8_UINT:
        return eR8G8_UINT;
    case DXGI_FORMAT_R8G8_SNORM:
        return eR8G8_SNORM;
    case DXGI_FORMAT_R8G8_SINT:
        return eR8G8_SINT;
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        return eR8G8B8A8_TYPELESS;
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        return eB8G8R8A8_TYPELESS;
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        return eB8G8R8X8_TYPELESS;

    case DXGI_FORMAT_R16_FLOAT:
        return eR16_FLOAT;
    case DXGI_FORMAT_R16G16_FLOAT:
        return eR16G16_FLOAT;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return eR16G16B16A16_FLOAT;
    case DXGI_FORMAT_R16_UNORM:
        return eR16_UNORM;
    case DXGI_FORMAT_R16G16_UNORM:
        return eR16G16_UNORM;
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        return eR16G16B16A16_UNORM;
    case DXGI_FORMAT_R16_TYPELESS:
        return eR16_TYPELESS;
    case DXGI_FORMAT_R16_UINT:
        return eR16_UINT;
    case DXGI_FORMAT_R16_SNORM:
        return eR16_SNORM;
    case DXGI_FORMAT_R16_SINT:
        return eR16_SINT;
    case DXGI_FORMAT_R16G16_TYPELESS:
        return eR16G16_TYPELESS;
    case DXGI_FORMAT_R16G16_UINT:
        return eR16G16_UINT;
    case DXGI_FORMAT_R16G16_SNORM:
        return eR16G16_SNORM;
    case DXGI_FORMAT_R16G16_SINT:
        return eR16G16_SINT;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        return eR16G16B16A16_TYPELESS;
    case DXGI_FORMAT_R16G16B16A16_UINT:
        return eR16G16B16A16_UINT;
    case DXGI_FORMAT_R16G16B16A16_SNORM:
        return eR16G16B16A16_SNORM;
    case DXGI_FORMAT_R16G16B16A16_SINT:
        return eR16G16B16A16_SINT;

    case DXGI_FORMAT_R32_FLOAT:
        return eR32_FLOAT;
    case DXGI_FORMAT_R32G32_FLOAT:
        return eR32G32_FLOAT;
    case DXGI_FORMAT_R32G32B32_FLOAT:
        return eR32G32B32_FLOAT;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return eR32G32B32A32_FLOAT;
    case DXGI_FORMAT_R32_UINT:
        return eR32_UINT;
    case DXGI_FORMAT_R32G32_UINT:
        return eR32G32_UINT;
    case DXGI_FORMAT_R32G32B32_UINT:
        return eR32G32B32_UINT;
    case DXGI_FORMAT_R32G32B32A32_UINT:
        return eR32G32B32A32_UINT;
    case DXGI_FORMAT_R32_TYPELESS:
        return eR32_TYPELESS;
    case DXGI_FORMAT_R32_SINT:
        return eR32_SINT;
    case DXGI_FORMAT_R32G32_TYPELESS:
        return eR32G32_TYPELESS;
    case DXGI_FORMAT_R32G32_SINT:
        return eR32G32_SINT;
    case DXGI_FORMAT_R32G32B32_TYPELESS:
        return eR32G32B32_TYPELESS;
    case DXGI_FORMAT_R32G32B32_SINT:
        return eR32G32B32_SINT;
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        return eR32G32B32A32_TYPELESS;
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return eR32G32B32A32_SINT;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        return eR10G10B10A2_TYPELESS;
    case DXGI_FORMAT_R10G10B10A2_UNORM:
        return eR10G10B10A2_UNORM;
    case DXGI_FORMAT_R10G10B10A2_UINT:
        return eR10G10B10A2_UINT;
    case DXGI_FORMAT_R11G11B10_FLOAT:
        return eR11G11B10_FLOAT;

    case DXGI_FORMAT_D32_FLOAT:
        return eD32_FLOAT;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return eD24_UNORM_S8_UINT;
    case DXGI_FORMAT_D16_UNORM:
        return eD16_UNORM;
    case DXGI_FORMAT_R24G8_TYPELESS:
        return eR24G8_TYPELESS;
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        return eR24_UNORM_X8_TYPELESS;
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return eX24_TYPELESS_G8_UINT;
    case DXGI_FORMAT_R32G8X24_TYPELESS:
        return eR32G8X24_TYPELESS;
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return eD32_FLOAT_S8X24_UINT;
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        return eR32_FLOAT_X8X24_TYPELESS;
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return eX32_TYPELESS_G8X24_UINT;

    case DXGI_FORMAT_A8_UNORM:
        return eA8_UNORM;
    case DXGI_FORMAT_R1_UNORM:
        return eR1_UNORM;
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        return eR9G9B9E5_SHAREDEXP;
    case DXGI_FORMAT_B5G6R5_UNORM:
        return eB5G6R5_UNORM;
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        return eB5G5R5A1_UNORM;
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        return eB4G4R4A4_UNORM;

    case DXGI_FORMAT_BC1_UNORM:
        return eBC1_UNORM;
    case DXGI_FORMAT_BC2_UNORM:
        return eBC2_UNORM;
    case DXGI_FORMAT_BC3_UNORM:
        return eBC3_UNORM;
    case DXGI_FORMAT_BC4_UNORM:
        return eBC4_UNORM;
    case DXGI_FORMAT_BC5_UNORM:
        return eBC5_UNORM;
    case DXGI_FORMAT_BC7_UNORM:
        return eBC7_UNORM;
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        return eBC1_UNORM_SRGB;
    case DXGI_FORMAT_BC2_UNORM_SRGB:
        return eBC2_UNORM_SRGB;
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        return eBC3_UNORM_SRGB;
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return eBC7_UNORM_SRGB;
    case DXGI_FORMAT_BC1_TYPELESS:
        return eBC1_TYPELESS;
    case DXGI_FORMAT_BC2_TYPELESS:
        return eBC2_TYPELESS;
    case DXGI_FORMAT_BC3_TYPELESS:
        return eBC3_TYPELESS;
    case DXGI_FORMAT_BC4_TYPELESS:
        return eBC4_TYPELESS;
    case DXGI_FORMAT_BC4_SNORM:
        return eBC4_SNORM;
    case DXGI_FORMAT_BC5_TYPELESS:
        return eBC5_TYPELESS;
    case DXGI_FORMAT_BC5_SNORM:
        return eBC5_SNORM;
    case DXGI_FORMAT_BC6H_TYPELESS:
        return eBC6H_TYPELESS;
    case DXGI_FORMAT_BC6H_UF16:
        return eBC6H_UF16;
    case DXGI_FORMAT_BC6H_SF16:
        return eBC6H_SF16;
    case DXGI_FORMAT_BC7_TYPELESS:
        return eBC7_TYPELESS;

    default:
        return eUnknown;
    }
}
