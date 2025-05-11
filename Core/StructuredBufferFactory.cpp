#include "StructuredBufferFactory.h"
#include"Buffer/StructuredBuffer.h"


void Quad::StructuredBufferFactory::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
	DescriptorHeapManagerMaster* descriptorHeapManagerMaster)
{
	mDevice = device;
	mDescriptorHeapManagerMaster = descriptorHeapManagerMaster;
}

Quad::StructuredBuffer* Quad::StructuredBufferFactory::CreateStructuredBuffer(UINT elementSize, UINT elementMaxNum)
{
	
	StructuredBuffer* structuredBuffer = new StructuredBuffer(mDevice, elementSize, elementMaxNum);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = elementMaxNum;
	srvDesc.Buffer.StructureByteStride = elementSize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	ViewIndex viewIndex = mDescriptorHeapManagerMaster->CreateSrv(structuredBuffer->mUploadBuffer->GetResource(), &srvDesc);
	structuredBuffer->SetViewIndex(viewIndex);


	return structuredBuffer;

	
	//여기서 생성시 srv를 바인딩해준다.


}
