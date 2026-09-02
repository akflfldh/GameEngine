#pragma once

#include "D3DGpuResourceManager/D3DGpuBuffer.h"
#include "D3DGpuResourceManager/GpuResourceDllMacro.h"
#include <vector>
namespace D3DGRM
{

class GPURESOURCE_MANAGER_API D3DGpuStructuredBuffer : public D3DGpuBuffer
{
  public:
    D3DGpuStructuredBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer, size_t elementNum, size_t elementSize);
    virtual ~D3DGpuStructuredBuffer();

    size_t GetElementwNum() const;

  private:
    size_t mElementNum;
    size_t mElementSize;
};
} // namespace D3DGRM
