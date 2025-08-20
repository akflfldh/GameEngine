#include "D3DGpuResourceManager/D3DGpuConstantBuffer.h"

D3DGRM::D3DGpuConstantBuffer::D3DGpuConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer,
                                                   size_t constantBufferNum, size_t constantBufferSize)
    : D3DGpuBuffer(buffer), mConstantBufferNum(constantBufferNum), mConstantBufferSize(constantBufferSize)
{
}

D3DGRM::D3DGpuConstantBuffer::~D3DGpuConstantBuffer() {}

void D3DGRM::D3DGpuConstantBuffer::SetDescriptorHandleVector(const std::vector<D3DDescriptorHandle> &handleVector)
{
    mDescriptorHandleVector = handleVector;
}

size_t D3DGRM::D3DGpuConstantBuffer::GetConstantBufferNum() const
{
    return mConstantBufferNum;
}
