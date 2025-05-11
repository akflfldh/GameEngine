
#include "Buffer/ConstantBuffer.h"
#include"Buffer/UploadBuffer.h"



Quad::ConstantBuffer::ConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device ,UINT elementSize , UINT elementNum)
{

	mUploadBuffer = new UploadBuffer(device, elementSize, elementNum, true);
	mElementMaxNum = elementNum;
	mElementSize = mUploadBuffer->GetElementBufferSize();

	mUploadCount = 0;


}




Quad::ConstantBuffer::~ConstantBuffer()
{
	
	
	delete mUploadBuffer;



}

ViewIndex Quad::ConstantBuffer::GetViewIndex(int elementIndex) const
{
	if (mElementMaxNum >= elementIndex)
		return InvalidViewIndex;


	return mViewIndexVector[elementIndex];
}

UINT Quad::ConstantBuffer::GetElementSize() const
{
	return mElementSize;
}

UINT Quad::ConstantBuffer::GetElementMaxNum() const
{
	return mElementMaxNum;
}

void Quad::ConstantBuffer::ResetUpLoadCount()
{
	mUploadCount = 0;
}

ViewIndex Quad::ConstantBuffer::UploadData(const void* data, UINT dataSize)
{
	mUploadBuffer->CopyData(mUploadCount,data, dataSize);
	mUploadCount++;
	return mViewIndexVector[mUploadCount - 1];

}

UINT Quad::ConstantBuffer::GetCurrentUploadCount()
{
	return mUploadCount;
}
