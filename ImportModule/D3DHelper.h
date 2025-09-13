#pragma once

#include <CommonHeader/GpuTypes.h>
#ifdef D3DX

#include <DirectXTex.h>
#include <d3d12.h>

GRM::ETextureType ConvertTextureType(DirectX::TEX_DIMENSION dimension);
GRM::ETextureFormat ConvertToDxgiFormat(DXGI_FORMAT dxgiFormat);

#endif