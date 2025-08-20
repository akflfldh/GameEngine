#include "D3DGpuResourceManager/D3DGpuTexture.h"

D3DGRM::D3DGpuTexture::D3DGpuTexture(Microsoft::WRL::ComPtr<ID3D12Resource> resource) : D3DGpuResource(resource) {}

D3DGRM::D3DGpuTexture::~D3DGpuTexture() {}
