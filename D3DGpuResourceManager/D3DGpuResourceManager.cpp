#include "D3DGpuResourceManager.h"
#include"D3DGpuBuffer.h"
#include"D3DGpuVertexBuffer.h"
#include"D3DGpuIndexBuffer.h"
#include"D3DGpuConstantBuffer.h"

#include"D3DGpuDescriptorHeapManager.h"

#include<d3dx12.h>


D3DGRM::D3DGpuResourceManager::D3DGpuResourceManager(ID3D12Device* device, ID3D12CommandQueue* commandQueue)
	:mDevice(device),mCommandQueue(commandQueue)
{
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf()));
	
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf()));

	mCurrentFence = 0;
	device->CreateFence(mCurrentFence, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf()));


	mCSUHeapManager = std::make_unique<D3DGpuDescriptorHeapManager>(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 5000);
	mRTVHeapManager = std::make_unique<D3DGpuDescriptorHeapManager>(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 100,D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	mDSVHeapManager = std::make_unique<D3DGpuDescriptorHeapManager>(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 100,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	mSMPHeapManager = std::make_unique<D3DGpuDescriptorHeapManager>(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 50);
	

	//HRESULT log 필요
}


D3DGRM::D3DGpuResourceManager::~D3DGpuResourceManager()
{
}

GRM::IGpuResource* D3DGRM::D3DGpuResourceManager::CreateBuffer(const GRM::BufferDesc& bufferDesc)
{

	D3DGpuBuffer* buffer = nullptr;
	//createBuffer()


	switch (bufferDesc.mBufferMemoryAccess)
	{
	case GRM::EBufferMemoryAccess::eGpuOnly:

		buffer = CreateDefaultBufferResource(bufferDesc);

		break;
	case GRM::EBufferMemoryAccess::eCpuWriteOnly:

		buffer = CreateUploadBufferResource(bufferDesc);

		break;
	}

	CreateBufferDescriptor(buffer, bufferDesc);
	FlushCommandQueue();


	return buffer;
}



GRM::IGpuResource* D3DGRM::D3DGpuResourceManager::Create2DTextureResource()
{
	return nullptr;
}

void D3DGRM::D3DGpuResourceManager::RegisterSwapChainBackBufferView(void* resoure)
{

	ID3D12Resource * swapChainBuffer = reinterpret_cast<ID3D12Resource*>(resoure);
	
	D3DDescriptorHandle rtvHandle =	mRTVHeapManager->Allocate();

	mDevice->CreateRenderTargetView(swapChainBuffer, nullptr, rtvHandle.mCpuDescriptorHandle);

	mSwapChainBackBufferHandleTable[swapChainBuffer] = rtvHandle;
}

void D3DGRM::D3DGpuResourceManager::ReleaseSwapChainBackBufferView(void* resoure)
{
	ID3D12Resource* swapChainBuffer = reinterpret_cast<ID3D12Resource*>(resoure);

	auto it = mSwapChainBackBufferHandleTable.find(swapChainBuffer);

	if (it == mSwapChainBackBufferHandleTable.end())
		return;


	mRTVHeapManager->Free(it->second);


	mSwapChainBackBufferHandleTable.erase(it);



}


D3DGRM::D3DGpuBuffer* D3DGRM::D3DGpuResourceManager::CreateDefaultBufferResource(const GRM::BufferDesc& bufferDesc)
{

	D3D12_HEAP_PROPERTIES heapProperties;
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 0;
	heapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.MipLevels = 1;
	resourceDesc.Width = bufferDesc.mBufferSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Alignment = 0;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource;

	mDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr, IID_PPV_ARGS(bufferResource.GetAddressOf()));
	

	//임시 복사용 업로드힙 버퍼 생성
	Microsoft::WRL::ComPtr<ID3D12Resource> tempUploadBufferResource;
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	mDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(tempUploadBufferResource.GetAddressOf()));

	D3D12_RANGE range = { 0,0 };
	void* pData = nullptr;
	tempUploadBufferResource->Map(0, &range, &pData);


	memcpy(pData, bufferDesc.mData, bufferDesc.mElementDataSize * bufferDesc.mElementDataNum);


	
	CD3DX12_RESOURCE_BARRIER uploadBufferToCopySourceState = CD3DX12_RESOURCE_BARRIER::Transition(tempUploadBufferResource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE);
	mCommandList->ResourceBarrier(1, &uploadBufferToCopySourceState);


	mCommandList->CopyResource(bufferResource.Get(), tempUploadBufferResource.Get());
	

	ID3D12CommandList* commandListArray[] = { mCommandList.Get()};
	mCommandQueue->ExecuteCommandLists(1, commandListArray);
	//후에 버퍼의 최종상태는 디스크립터 처리단계에서 전환한다. 

	//fence 
	FlushCommandQueue();

	mCommandList->Reset(mCommandAllocator.Get(), nullptr);
	mCommandAllocator->Reset();



	//CD3DX12_RESOURCE_BARRIER::Transition(bufferResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_C)
	//mCommandList->ResourceBarrier(1,)


	D3DGpuBuffer* gpuBuffer =  CreateBuffer(bufferResource.Get(), bufferDesc);
	return gpuBuffer;
}

D3DGRM::D3DGpuBuffer* D3DGRM::D3DGpuResourceManager::CreateUploadBufferResource(const GRM::BufferDesc& bufferDesc)
{


	D3D12_HEAP_PROPERTIES heapProperties;
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 0;
	heapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.MipLevels = 1;
	resourceDesc.Width = bufferDesc.mBufferSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Alignment = 0;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource;

	mDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(bufferResource.GetAddressOf()));



	D3D12_RANGE range = { 0,0 };
	void* pData = nullptr;
	bufferResource->Map(0, &range, &pData);


	memcpy(pData, bufferDesc.mData, bufferDesc.mElementDataNum*bufferDesc.mElementDataSize);


	


	D3DGpuBuffer* gpuBuffer = CreateBuffer(bufferResource.Get(), bufferDesc);
	return gpuBuffer;
}


D3DGRM::D3DGpuBuffer* D3DGRM::D3DGpuResourceManager::CreateBuffer(ID3D12Resource* resource, const GRM::BufferDesc& bufferDesc)
{


	D3DGpuBuffer* buffer = nullptr;
	switch (bufferDesc.mBufferUsage)
	{
	case GRM::EBufferUsage::eConstantBuffer:

		buffer = new D3DGpuConstantBuffer(resource,bufferDesc.mElementDataNum,bufferDesc.mElementDataSize);

		break;
	case GRM::EBufferUsage::eVertexBuffer:

		buffer = new D3DGpuVertexBuffer(resource,bufferDesc.mElementDataNum,bufferDesc.mElementDataSize);

		break;

	case GRM::EBufferUsage::eIndexBuffer:

		buffer = new D3DGpuIndexBuffer(resource,bufferDesc.mElementDataNum,bufferDesc.mElementDataSize);
		break;
	}


	return buffer;
}

void D3DGRM::D3DGpuResourceManager::FlushCommandQueue()
{
	mCurrentFence++;

	mCommandQueue->Signal(mFence.Get(), mCurrentFence);


	if (mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

		mFence->SetEventOnCompletion(mCurrentFence, eventHandle);

		WaitForSingleObject(eventHandle, INFINITE);

		CloseHandle(eventHandle);
	}

}

void D3DGRM::D3DGpuResourceManager::CreateBufferDescriptor(D3DGpuBuffer* buffer, const GRM::BufferDesc& bufferDesc)
{


	D3DGpuResource* resoure = static_cast<D3DGpuResource*>(buffer);

	//descriptor 에 관련된부분
	//적절한 최종상태전이 + descriptor생성
	switch (bufferDesc.mBufferUsage)
	{
	case GRM::EBufferUsage::eConstantBuffer:

	{
		D3DGpuConstantBuffer* constantBuffer = static_cast<D3DGpuConstantBuffer*>(buffer);
		std::vector<D3DDescriptorHandle> descriptorHandleVector(bufferDesc.mElementDataNum);
		for (size_t bufferIndex = 0; bufferIndex < bufferDesc.mElementDataNum; ++bufferIndex)
		{

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = buffer->GetResource()->GetGPUVirtualAddress() + bufferIndex* bufferDesc.mElementDataSize;	//CaculateConstantSize(mElementDataSize)

			cbvDesc.SizeInBytes = 0;//CaculateConstantSize(mElementDataSize)


			descriptorHandleVector[bufferIndex] = mCSUHeapManager->Allocate();
			mDevice->CreateConstantBufferView(&cbvDesc, descriptorHandleVector[bufferIndex].mCpuDescriptorHandle);

			
		
			if (bufferDesc.mBufferMemoryAccess == GRM::EBufferMemoryAccess::eGpuOnly)
			{
				D3D12_RESOURCE_BARRIER constantBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(buffer->GetResource().Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

				mCommandList->ResourceBarrier(1, &constantBufferTransition);
			}

		}

		constantBuffer->SetDescriptorHandleVector(descriptorHandleVector);


	}

		break;

	case GRM::EBufferUsage::eVertexBuffer:
	{


		if (bufferDesc.mBufferMemoryAccess == GRM::EBufferMemoryAccess::eGpuOnly)
		{
			D3D12_RESOURCE_BARRIER vertexBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(buffer->GetResource().Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

			mCommandList->ResourceBarrier(1, &vertexBufferTransition);
		}



	}

		break;
	case GRM::EBufferUsage::eIndexBuffer:
	{

		if (bufferDesc.mBufferMemoryAccess == GRM::EBufferMemoryAccess::eGpuOnly)
		{
			D3D12_RESOURCE_BARRIER indexBufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(buffer->GetResource().Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_INDEX_BUFFER);

			mCommandList->ResourceBarrier(1, &indexBufferTransition);
		}

	}
		break;
	case GRM::EBufferUsage::eDefault:
	{


	}
		break;
	}

}
