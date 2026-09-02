#pragma once

#include "D3DGpuResourceManager/D3DGpuBuffer.h"
#include "D3DGpuResourceManager/GpuResourceDllMacro.h"
#include <vector>
namespace D3DGRM
{
// 상수버퍼는 하나의 거대한 버퍼를  할당해서 연속된 작은 상수버퍼로 사용한다.

class GPURESOURCE_MANAGER_API D3DGpuConstantBuffer : public D3DGpuBuffer
{
  public:
    D3DGpuConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer, size_t constantBufferNum,
                         size_t constantBufferSize);
    virtual ~D3DGpuConstantBuffer();

    void SetDescriptorHandleVector(const std::vector<D3DDescriptorHandle> &handleVector);

    D3DDescriptorHandle GetConstantDescriptorHandle(uint32_t index) const;

    size_t GetConstantBufferNum() const;
    size_t GetConstnatBufferSize() const;

  private:
    std::vector<D3DDescriptorHandle> mDescriptorHandleVector; // 상수버퍼의 개수만큼존재한다.

    size_t mConstantBufferNum;
    size_t mConstantBufferSize;
};

} // namespace D3DGRM
