#pragma once
#ifdef D3DX

#include <CoreDevice/CoreDeviceHeader.h>
#include <Include/DirectTexX/DirectXTex.h>
#include <d3d12.h>

GRM::ETextureType ConvertTextureType(DirectX::TEX_DIMENSION dimension);
GRM::ETextureFormat ConvertToDxgiFormat(DXGI_FORMAT dxgiFormat);

#endif