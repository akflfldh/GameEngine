#include "D3DGpuStructuredBuffer.h"

D3DGRM::D3DGpuStructuredBuffer::D3DGpuStructuredBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer, size_t elementNum,
                                                       size_t elementSize)
    : D3DGpuBuffer(buffer), mElementNum(elementNum), mElementSize(elementSize)
{
}

D3DGRM::D3DGpuStructuredBuffer::~D3DGpuStructuredBuffer() {}

size_t D3DGRM::D3DGpuStructuredBuffer::GetElementwNum() const
{
    return mElementNum;
}