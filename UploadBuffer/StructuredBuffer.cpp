#include "Buffer/StructuredBuffer.h"


Quad::StructuredBuffer::StructuredBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device, UINT elementSize, UINT elementMaxNum)
	:mElementSize(elementSize),mElementMaxNum(elementMaxNum), mUploadBuffer(nullptr) , mViewIndex(0), mUploadCount(0)
{
	mUploadBuffer = new UploadBuffer(device, elementSize, elementMaxNum, false);


}

void Quad::StructuredBuffer::SetViewIndex(ViewIndex index)
{
	mViewIndex = index;

}

void Quad::StructuredBuffer::CopyData(UINT index, const void* srcData)
{
	//mUploadBuffer.CopyData(index, srcData,);
}

ViewIndex Quad::StructuredBuffer::GetViewIndex() const
{
	return mViewIndex;
}

void Quad::StructuredBuffer::UploadData(const void* data)
{
	
	mUploadBuffer->CopyData(mUploadCount, data, mElementSize);
	mUploadCount++;
}

UINT Quad::StructuredBuffer::GetElementSize() const
{
	return mElementSize;
}

void Quad::StructuredBuffer::ResetUploadCount()
{
	mUploadCount = 0;
}
