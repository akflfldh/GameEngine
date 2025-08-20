#pragma once

#include "D3DGpuResourceManager/D3DGpuBuffer.h"
#include "D3DGpuResourceManager/GpuResourceDllMacro.h"
namespace D3DGRM
{

class GPURESOURCE_MANAGER_API D3DGpuIndexBuffer : public D3DGpuBuffer
{
  public:
    D3DGpuIndexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> bufffer, size_t indexNum, size_t indexSize);
    virtual ~D3DGpuIndexBuffer();

    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

  private:
    size_t mIndexNum;
    size_t mIndexSize;
};

} // namespace D3DGRM
