#include "D3DGpuResourceManager.h"
#include"D3DGpuBuffer.h"
#include"D3DGpuVertexBuffer.h"
#include"D3DGpuIndexBuffer.h"
#include"D3DGpuConstantBuffer.h"
#include"D3DGpuTexture.h"

#include"D3DGpuDescriptorHeapManager.h"
#include"GRMPtr.h"

#include<d3dx12.h>



D3DGRM::D3DGpuResourceManager::D3DGpuResourceManager(Microsoft::WRL::ComPtr < ID3D12Device> device, Microsoft::WRL::ComPtr < ID3D12CommandQueue>  commandQueue)
	:mDevice(device),mCommandQueue(commandQueue)
{
	HRESULT ret =device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf()));
	
	ret =device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf()));

	mCommandList->Close();

	mCurrentFence = 0;
	ret =device->CreateFence(mCurrentFence, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf()));


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

GRM::GRMPtr D3DGRM::D3DGpuResourceManager::CreateBuffer(const GRM::BufferDesc& bufferDesc)
{

	D3DGpuBuffer* buffer = nullptr;
	//createBuffer()


	switch (bufferDesc.mBufferMemoryAccess)
	{
	case GRM::EBufferMemoryAccess::eGpuOnly:

		buffer = CreateDefaultBuffer(bufferDesc);

		break;
	case GRM::EBufferMemoryAccess::eCpuWriteOnly:

		buffer = CreateUploadBuffer(bufferDesc);

		break;
	}

	CreateBufferDescriptor(buffer, bufferDesc);
	FlushCommandQueue();


	return buffer;
}


//텍스처는 외부에서 , width,height,정보 ,format정보 가 기본
//후에 밉맵레벨,등등 을 더 추가로 받을수도있을거같다.
GRM::GRMPtr D3DGRM::D3DGpuResourceManager::CreateTexture(const GRM::TextureDesc& textureDesc)
{

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(textureDesc);

	if (textureResource == nullptr)
		return nullptr;

	//data upload 
	//Upload Data
	if(!textureDesc.mScratchImage.mMemory.empty())
		UploadTextureData(textureResource, textureDesc, D3D12_RESOURCE_STATE_GENERIC_READ);

	//실패시 예외처리 를 하는 어떤시스템이 필요할것이다.
	D3DGpuTexture* texture = GenerateTextureObject(textureResource);
	if (texture == nullptr)
		return nullptr;
	


	CreateTextureDescriptor(texture, textureDesc);
	texture->SetResourceState(D3D12_RESOURCE_STATE_GENERIC_READ);

	return texture;
}

void D3DGRM::D3DGpuResourceManager::ResizeSwapChainBackBuffer(const GRM::GRMPtr & resource, void* innerResoure)
{
	ID3D12Resource* swapChainBuffer = reinterpret_cast<ID3D12Resource*>(innerResoure);
	D3DGpuTexture* texture = static_cast<D3DGpuTexture*>(resource.getResource());

	D3DDescriptorHandle rtvHandle = mRTVHeapManager->Allocate();
	mDevice->CreateRenderTargetView(swapChainBuffer, nullptr, rtvHandle.mCpuDescriptorHandle);
	texture->SetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, rtvHandle);


	D3DGpuResource* generalResource  = static_cast<D3DGpuResource*>(resource.getResource());
	generalResource->mResource = swapChainBuffer;


}

GRM::GRMPtr D3DGRM::D3DGpuResourceManager::RegisterSwapChainBackBuffer(void* resoure)
{

	ID3D12Resource * swapChainBuffer = reinterpret_cast<ID3D12Resource*>(resoure);
	
	D3DDescriptorHandle rtvHandle =	mRTVHeapManager->Allocate();



	//리소스가 이미있으니 rtvDesc를 설정하지않아도 되니 개선할것  - 일반 텍스처생성시에도
	mDevice->CreateRenderTargetView(swapChainBuffer, nullptr, rtvHandle.mCpuDescriptorHandle);

	D3DGpuTexture* texture = new D3DGpuTexture(swapChainBuffer);

	texture->SetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, rtvHandle);

	//mSwapChainBackBufferHandleTable[swapChainBuffer] = rtvHandle;

	
	return texture;
}

void D3DGRM::D3DGpuResourceManager::ReleaseSwapChainBackBuffer(const GRM::GRMPtr& backbuffer)
{
	D3DGpuTexture* texture = static_cast<D3DGpuTexture*>(backbuffer.getResource());

	//auto it = mSwapChainBackBufferHandleTable.find(swapChainBuffer);

	//if (it == mSwapChainBackBufferHandleTable.end())
	//	return;

	D3DDescriptorHandle handle;
	texture->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, handle);
	mRTVHeapManager->Free(handle);

	texture->RemoveDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV);
	
	D3DGpuResource* generalResource = static_cast<D3DGpuResource*>(backbuffer.getResource());

	generalResource->mResource.Reset();
//	mSwapChainBackBufferHandleTable.erase(it);



}

void D3DGRM::D3DGpuResourceManager::ChangeTextureData(const GRM::GRMPtr& texture, const GRM::TextureDesc& textureDesc)
{
	if (texture.getResource() == nullptr)
		return;

	D3DGRM::D3DGpuResource* generalResource = static_cast<D3DGRM::D3DGpuResource*>(texture.getResource());
	generalResource->mResource.Reset();
	
	Microsoft::WRL::ComPtr<ID3D12Resource> textureInnerResource =  CreateTextureResource(textureDesc);
	
	D3DGRM::D3DGpuTexture* textureResource = static_cast<D3DGRM::D3DGpuTexture*>(texture.getResource());

	textureResource->RemoveDescriptorHandleAll();
	CreateTextureDescriptor(textureResource, textureDesc);
	
	generalResource->SetResourceState(D3D12_RESOURCE_STATE_GENERIC_READ);

}

bool D3DGRM::D3DGpuResourceManager::UploadBufferData(const GRM::GRMPtr& buffer, void* data, size_t elementSize, size_t elementNum, size_t bufferOffset)
{

	GRM::IGpuResource* gpuResource = buffer.getResource();

	if (gpuResource == nullptr || data == nullptr)
		return false;

	GRM::EGpuResourceType resourceType = gpuResource->GetGpuResourceType();
	if (resourceType != GRM::EGpuResourceType::eBuffer)
	{
		//log
		return false;
	}


	//buffer라면

	

	D3DGRM::D3DGpuBuffer* d3dBuffer = static_cast<D3DGpuBuffer*>(gpuResource);
	D3D12_HEAP_PROPERTIES heapProperties;
	Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource = d3dBuffer->GetResource();
	d3dResource->GetHeapProperties(&heapProperties, nullptr);

	//+개선할것 : 상수버퍼인지도 확인필요, buffer의 상태추적이필요함 (generic read, ... 상태등등)

	//upload heap 에존재하는 buffer ,default heap에존재하는 buffer를 구분해서 수행한다.
	if (heapProperties.Type == D3D12_HEAP_TYPE_UPLOAD)
	{
	
		UploadBufferData(d3dResource, data, elementSize, elementNum, bufferOffset);

	}
	else if (heapProperties.Type == D3D12_HEAP_TYPE_DEFAULT)
	{
		mCommandAllocator->Reset();
		mCommandList->Reset(mCommandAllocator.Get(), nullptr);

		//임시 upload heap buffer 생성
		GRM::BufferDesc tempUploadBufferDesc;
		tempUploadBufferDesc.mData = data;
		tempUploadBufferDesc.mElementDataNum = elementNum;
		tempUploadBufferDesc.mElementDataSize = elementSize;
		tempUploadBufferDesc.mBufferSize = elementNum * elementSize;

		Microsoft::WRL::ComPtr<ID3D12Resource> tempBuffer = CreateUploadBufferResource(tempUploadBufferDesc);
		UploadBufferData(tempBuffer, data, elementSize, elementNum, bufferOffset);

		//복사

		//상태변이 PreTransition
		D3D12_RESOURCE_STATES preResourceState = d3dBuffer->GetResourceState();

		CD3DX12_RESOURCE_BARRIER preTransition[1] = {
			CD3DX12_RESOURCE_BARRIER::Transition(d3dResource.Get(),preResourceState,
				D3D12_RESOURCE_STATE_COPY_DEST)
		};
		
		mCommandList->ResourceBarrier(1, preTransition);
		//복사
		//mCommandList->CopyResource(d3dResource.Get(), tempBuffer.Get());
		mCommandList->CopyBufferRegion(d3dResource.Get(), bufferOffset, tempBuffer.Get(), 0, tempUploadBufferDesc.mBufferSize);

		//상태변이 PostTransition
		D3D12_RESOURCE_STATES finalState = D3D12_RESOURCE_STATE_GENERIC_READ;
		CD3DX12_RESOURCE_BARRIER postTransition[1] = {
			CD3DX12_RESOURCE_BARRIER::Transition(d3dResource.Get(),D3D12_RESOURCE_STATE_COPY_DEST,
				finalState)
		};
		mCommandList->ResourceBarrier(1, postTransition);


		mCommandList->Close();
		ID3D12CommandList* commandList[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(1, commandList);
		//동기화
		FlushCommandQueue();
		d3dBuffer->SetResourceState(finalState);

	}
	else
	{
		//log 
		return false;
	}



	return true;
}


GRM::GRMPtr D3DGRM::D3DGpuResourceManager::CreateSampler()
{
	D3DGRM::D3DGpuResource* sampler = new D3DGRM::D3DGpuResource(nullptr);

	D3D12_SAMPLER_DESC smpDesc;
	smpDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	smpDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	smpDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	smpDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	smpDesc.MipLODBias = 0.0f;
	smpDesc.MaxAnisotropy = 1;
	smpDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	smpDesc.BorderColor[0] = 1.0f;
	smpDesc.BorderColor[1] = 1.0f;
	smpDesc.BorderColor[2] = 1.0f;
	smpDesc.BorderColor[3] = 1.0f;
	smpDesc.MinLOD = 0.0f;
	smpDesc.MaxLOD = D3D12_FLOAT32_MAX;


	D3DDescriptorHandle handle =	mSMPHeapManager->Allocate();
	mDevice->CreateSampler(&smpDesc, handle.mCpuDescriptorHandle);


	sampler->SetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eSMP, handle);


	return sampler;


	
}


D3DGRM::D3DGpuBuffer* D3DGRM::D3DGpuResourceManager::CreateDefaultBuffer(const GRM::BufferDesc& bufferDesc)
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
	
	//복사할 데이터가있을때
	if (bufferDesc.mData != nullptr)
	{
		//임시 복사용 업로드힙 버퍼 생성
		Microsoft::WRL::ComPtr<ID3D12Resource> tempUploadBufferResource;
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		mDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(tempUploadBufferResource.GetAddressOf()));

		D3D12_RANGE range = { 0,0 };
		void* pData = nullptr;
		tempUploadBufferResource->Map(0, &range, &pData);



		//uploadBuffer로복사
		memcpy(pData, bufferDesc.mData, bufferDesc.mElementDataSize * bufferDesc.mElementDataNum);

		CD3DX12_RESOURCE_BARRIER uploadBufferToCopySourceState = CD3DX12_RESOURCE_BARRIER::Transition(tempUploadBufferResource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE);
		mCommandList->ResourceBarrier(1, &uploadBufferToCopySourceState);


		//default buffer 로 복사
		mCommandList->CopyResource(bufferResource.Get(), tempUploadBufferResource.Get());

	}

	D3D12_RESOURCE_STATES finalState = D3D12_RESOURCE_STATE_GENERIC_READ;
	CD3DX12_RESOURCE_BARRIER postTransition = CD3DX12_RESOURCE_BARRIER::Transition(bufferResource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, finalState);
	mCommandList->ResourceBarrier(1, &postTransition);


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
	if (gpuBuffer)
		gpuBuffer->SetResourceState(finalState);


	return gpuBuffer;
}

D3DGRM::D3DGpuBuffer* D3DGRM::D3DGpuResourceManager::CreateUploadBuffer(const GRM::BufferDesc& bufferDesc)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource =nullptr;
	bufferResource = CreateUploadBufferResource(bufferDesc);

	UploadBufferData(bufferResource, bufferDesc.mData, bufferDesc.mElementDataSize, bufferDesc.mElementDataNum);

	D3DGpuBuffer* gpuBuffer = CreateBuffer(bufferResource.Get(), bufferDesc);
	if (gpuBuffer)
		gpuBuffer->SetResourceState(D3D12_RESOURCE_STATE_GENERIC_READ);

	return gpuBuffer;
}

Microsoft::WRL::ComPtr<ID3D12Resource> D3DGRM::D3DGpuResourceManager::CreateTextureResource(const GRM::TextureDesc& textureDesc)
{


	const GRM::TexMetaData& texMetaData = textureDesc.mScratchImage.mMetadata;





	D3D12_HEAP_PROPERTIES heapProperties;
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 0;
	heapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_FLAGS resourceFlags = ConvertToResourceFlags(textureDesc.mTextureUsage);

	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = ConvertToResourceDimension(texMetaData.mDimension);
	resourceDesc.Format = ConvertToDxgiFormat(texMetaData.mFormat);
	resourceDesc.MipLevels = texMetaData.mMipLevels;
	resourceDesc.Width = texMetaData.mWidth;
	resourceDesc.Height = texMetaData.mHeight;
	resourceDesc.DepthOrArraySize = texMetaData.mDepth;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Alignment = 0;
	resourceDesc.Flags = resourceFlags;


	std::unique_ptr<D3D12_CLEAR_VALUE> clearValue =nullptr;

	if (resourceFlags != D3D12_RESOURCE_FLAG_NONE)
	{

		clearValue = std::make_unique<D3D12_CLEAR_VALUE>();
		if (resourceFlags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
		{
			clearValue->DepthStencil.Depth = textureDesc.mOptimizedClearValue.mOptimizedDepthStencilValue.mOptimizedClearDepth;
			clearValue->DepthStencil.Stencil = textureDesc.mOptimizedClearValue.mOptimizedDepthStencilValue.mOptimizedClearStencil;
		}
		else {
			clearValue->Color[0] = textureDesc.mOptimizedClearValue.mOptimizedColor[0];
			clearValue->Color[1] = textureDesc.mOptimizedClearValue.mOptimizedColor[1];
			clearValue->Color[2] = textureDesc.mOptimizedClearValue.mOptimizedColor[2];
			clearValue->Color[3] = textureDesc.mOptimizedClearValue.mOptimizedColor[3];
		}
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource =nullptr;
	HRESULT result = mDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		clearValue.get(), IID_PPV_ARGS(textureResource.GetAddressOf()));


	return textureResource;

}

void D3DGRM::D3DGpuResourceManager::UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> textureResource, const GRM::TextureDesc& textureDesc, D3D12_RESOURCE_STATES preState)
{

	FlushCommandQueue();
	HRESULT ret =mCommandAllocator->Reset();

	mCommandList->Reset(mCommandAllocator.Get(),nullptr);


	D3D12_RESOURCE_DESC resourceDesc =	textureResource->GetDesc();


	uint32_t subResourceNum = resourceDesc.MipLevels * (resourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D) ? 1 : resourceDesc.DepthOrArraySize;


	UINT64 uploadTotalSize = GetRequiredIntermediateSize(textureResource.Get(), 0, subResourceNum);


	GRM::BufferDesc uploadBufferDesc;
	uploadBufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eCpuWriteOnly;
	uploadBufferDesc.mBufferSize = uploadTotalSize;

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer = CreateUploadBufferResource(uploadBufferDesc);;


	std::vector<D3D12_SUBRESOURCE_DATA> subResourceData(textureDesc.mScratchImage.mimagesNum);
	//image의 순서 
		//gpu sub resource 순서가 중요하다 - 더 자세히볼것
		//단 Directx에서 제공하는 헬퍼함수를 사용해서 가져왔을것이기에 
		//순서가이미 올바르게되어있을것이다.
	for (UINT subResourceIndex = 0; subResourceIndex < subResourceData.size(); ++subResourceIndex)
	{
		subResourceData[subResourceIndex].pData = textureDesc.mScratchImage.mImages[subResourceIndex].mPixels;
		subResourceData[subResourceIndex].RowPitch = textureDesc.mScratchImage.mImages[subResourceIndex].mRowPitch;
		subResourceData[subResourceIndex].SlicePitch = textureDesc.mScratchImage.mImages[subResourceIndex].mSlicePitch;
	}


	D3D12_RESOURCE_BARRIER texPreTransition= CD3DX12_RESOURCE_BARRIER::Transition(textureResource.Get(), preState, D3D12_RESOURCE_STATE_COPY_DEST);
	mCommandList->ResourceBarrier(1, &texPreTransition);

	UpdateSubresources(mCommandList.Get(), textureResource.Get(), uploadBuffer.Get(), 0, 0, subResourceNum, subResourceData.data());




	D3D12_RESOURCE_BARRIER texPostTransition = CD3DX12_RESOURCE_BARRIER::Transition(textureResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, preState);
	mCommandList->ResourceBarrier(1, &texPostTransition);




	HRESULT result = mCommandList->Close();

	if (FAILED(result))
	{
		//log
		return;
	}


	ID3D12CommandList* commandLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(1, commandLists);


	

	FlushCommandQueue();




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
		}

		constantBuffer->SetDescriptorHandleVector(descriptorHandleVector);


	}

		break;

	case GRM::EBufferUsage::eVertexBuffer:
	{

		TransitionResourceState(buffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	}

		break;
	case GRM::EBufferUsage::eIndexBuffer:
	{
		TransitionResourceState(buffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);
	}
		break;
	case GRM::EBufferUsage::eDefault:
	{


	}
		break;
	}

}



void D3DGRM::D3DGpuResourceManager::CreateTextureDescriptor(D3DGRM::D3DGpuTexture* texture, const GRM::TextureDesc& textureDesc)
{

	switch (textureDesc.mTextureUsage)
	{

	case GRM::ETextureUsage::eShaderResource:

	{
		CreateSrvHandle(texture, textureDesc);

	}
	break;

	case GRM::ETextureUsage::eRenderTarget:

	{
		CreateRtvHandle(texture, textureDesc);
	}
	break;
	case GRM::ETextureUsage::eDepthStencil:

	{
		CreateDsvHandle(texture, textureDesc);
	}
	break;
	case GRM::ETextureUsage::eRenderTargetShaderResource:

	{
		CreateRtvHandle(texture, textureDesc);
		CreateSrvHandle(texture, textureDesc);
	}
	break;
	}


}

void D3DGRM::D3DGpuResourceManager::CreateRtvHandle(D3DGRM::D3DGpuTexture* texture, const GRM::TextureDesc& textureDesc)
{

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = ConvertToDxgiFormat(textureDesc.mScratchImage.mMetadata.mFormat);
	rtvDesc.ViewDimension = ConvertToRTVDimension(textureDesc.mScratchImage.mMetadata.mDimension);
	
	
	switch (rtvDesc.ViewDimension)
	{
	case D3D12_RTV_DIMENSION_UNKNOWN:
		// 처리할 수 없는 뷰
		break;
	case D3D12_RTV_DIMENSION_TEXTURE1D:
		// 1D 텍스처에 대한 RTV
		break;

	case D3D12_RTV_DIMENSION_TEXTURE1DARRAY:
		// 1D 텍스처 배열에 대한 RTV
		break;

	case D3D12_RTV_DIMENSION_TEXTURE2D:
		// 2D 텍스처에 대한 RTV
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		break;

	case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
		// 2D 텍스처 배열에 대한 RTV
		break;

	case D3D12_RTV_DIMENSION_TEXTURE2DMS:
		// 멀티샘플링 2D 텍스처에 대한 RTV
		break;

	case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
		// 멀티샘플링 2D 텍스처 배열에 대한 RTV
		break;

	case D3D12_RTV_DIMENSION_TEXTURE3D:
		// 3D 텍스처에 대한 RTV
		break;

	default:
		// 정의되지 않은 경우
		break;
	}
	
	//SetRTVResourceTypeDesc(rtvDesc, textureDesc);

	D3DGpuResource* resource = static_cast<D3DGpuResource*>(texture);
	
	D3DDescriptorHandle handle = mRTVHeapManager->Allocate();
	mDevice->CreateRenderTargetView(resource->GetResource().Get(), &rtvDesc, handle.mCpuDescriptorHandle);

	//핸들설정
	texture->SetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, handle);

}

void D3DGRM::D3DGpuResourceManager::CreateSrvHandle(D3DGpuTexture* texture, const GRM::TextureDesc& textureDesc)
{

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = ConvertToDxgiFormat(textureDesc.mScratchImage.mMetadata.mFormat);
	srvDesc.ViewDimension = ConvertToSRVDimension(textureDesc.mScratchImage.mMetadata.mDimension);
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch (srvDesc.ViewDimension)
	{
	case D3D12_SRV_DIMENSION_UNKNOWN:
		// 처리할 수 없는 뷰
		break;

	case D3D12_SRV_DIMENSION_BUFFER:
		// 버퍼에 대한 SRV
		break;

	case D3D12_SRV_DIMENSION_TEXTURE1D:
		// 1D 텍스처에 대한 SRV
		srvDesc.Texture1D.MipLevels = 1;
		srvDesc.Texture1D.MostDetailedMip = 0;
		break;

	case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
		// 1D 텍스처 배열에 대한 SRV
		srvDesc.Texture1DArray.MipLevels = 1;
		srvDesc.Texture1DArray.MostDetailedMip = 0;
		srvDesc.Texture1DArray.ArraySize = 1;
		srvDesc.Texture1DArray.FirstArraySlice = 0;
		break;

	case D3D12_SRV_DIMENSION_TEXTURE2D:
		// 2D 텍스처에 대한 SRV
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.PlaneSlice = 0;
		break;

	case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
		// 2D 텍스처 배열에 대한 SRV
		srvDesc.Texture2DArray.MipLevels = 1;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.ArraySize = 1;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.PlaneSlice = 0;
		break;

	case D3D12_SRV_DIMENSION_TEXTURE2DMS:
		// 멀티샘플링 2D 텍스처에 대한 SRV
		break;

	case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
		// 멀티샘플링 2D 텍스처 배열에 대한 SRV
		srvDesc.Texture2DMSArray.ArraySize = 1;
		srvDesc.Texture2DMSArray.FirstArraySlice = 0;
		break;

	case D3D12_SRV_DIMENSION_TEXTURE3D:
		// 3D 텍스처에 대한 SRV
		srvDesc.Texture3D.MipLevels = 1;
		srvDesc.Texture3D.MostDetailedMip = 0;
		break;

	case D3D12_SRV_DIMENSION_TEXTURECUBE:
		// 큐브 텍스처에 대한 SRV
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.MostDetailedMip = 0;
		break;

	case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
		// 큐브 텍스처 배열에 대한 SRV
		srvDesc.TextureCubeArray.MipLevels = 1;
		srvDesc.TextureCubeArray.MostDetailedMip = 0;
		srvDesc.TextureCubeArray.First2DArrayFace = 0;
		srvDesc.TextureCubeArray.NumCubes = 1;
		break;

	default:
		// 정의되지 않은 경우
		break;
	}



	D3DGpuResource* resource = static_cast<D3DGpuResource*>(texture);

	D3DDescriptorHandle handle = mCSUHeapManager->Allocate();
	mDevice->CreateShaderResourceView(resource->GetResource().Get(), &srvDesc, handle.mCpuDescriptorHandle);
	HRESULT ret = mDevice->GetDeviceRemovedReason();
	//핸들설정
	texture->SetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eSRV, handle);




}

void D3DGRM::D3DGpuResourceManager::CreateDsvHandle(D3DGpuTexture* texture, const GRM::TextureDesc& textureDesc)
{


	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = ConvertToDxgiFormat(textureDesc.mScratchImage.mMetadata.mFormat);
	dsvDesc.ViewDimension = ConvertToDSVDimension(textureDesc.mScratchImage.mMetadata.mDimension);
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;



	switch (dsvDesc.ViewDimension)
	{
	case D3D12_DSV_DIMENSION_UNKNOWN:
		// 처리할 수 없는 뷰
		break;

	case D3D12_DSV_DIMENSION_TEXTURE1D:
		// 1D 텍스처에 대한 DSV
		dsvDesc.Texture1D.MipSlice = 0;
		break;

	case D3D12_DSV_DIMENSION_TEXTURE1DARRAY:
		// 1D 텍스처 배열에 대한 DSV
		dsvDesc.Texture1DArray.MipSlice = 0;
		dsvDesc.Texture1DArray.ArraySize = 1;
		dsvDesc.Texture1DArray.FirstArraySlice = 0;
		break;

	case D3D12_DSV_DIMENSION_TEXTURE2D:
		// 2D 텍스처에 대한 DSV
		dsvDesc.Texture2D.MipSlice = 0;
		break;

	case D3D12_DSV_DIMENSION_TEXTURE2DARRAY:
		// 2D 텍스처 배열에 대한 DSV
		dsvDesc.Texture2DArray.MipSlice = 0;
		dsvDesc.Texture2DArray.ArraySize = 1;
		dsvDesc.Texture2DArray.FirstArraySlice = 0;
		break;

	case D3D12_DSV_DIMENSION_TEXTURE2DMS:
		// 멀티샘플링 2D 텍스처에 대한 DSV
		// 별도 설정 없음
		break;

	case D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY:
		// 멀티샘플링 2D 텍스처 배열에 대한 DSV
		dsvDesc.Texture2DMSArray.ArraySize = 1;
		dsvDesc.Texture2DMSArray.FirstArraySlice = 0;
		break;

	default:
		// 정의되지 않은 경우
		break;
	}




	D3DGpuResource* resource = static_cast<D3DGpuResource*>(texture);

	D3DDescriptorHandle handle = mDSVHeapManager->Allocate();
	mDevice->CreateDepthStencilView(resource->GetResource().Get(), &dsvDesc, handle.mCpuDescriptorHandle);

	//핸들설정
	texture->SetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eDSV, handle);




}

D3DGRM::D3DGpuTexture* D3DGRM::D3DGpuResourceManager::GenerateTextureObject(Microsoft::WRL::ComPtr<ID3D12Resource> textureResource)
{
	D3DGpuTexture* texture = new D3DGpuTexture(textureResource);
	return texture;
}




Microsoft::WRL::ComPtr<ID3D12Resource> D3DGRM::D3DGpuResourceManager::CreateUploadBufferResource(const GRM::BufferDesc& bufferDesc)
{

	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource = nullptr;

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




	HRESULT result =mDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(bufferResource.GetAddressOf()));

	if (FAILED(result))
	{
		//log
		return nullptr;
	}


	//if (bufferDesc.mData != nullptr)
	//{

	//	D3D12_RANGE range = { 0,0 };
	//	void* pData = nullptr;
	//	result = bufferResource->Map(0, &range, &pData);
	//	if (FAILED(result))
	//	{
	//		//log
	//		return nullptr;
	//	}
	//	memcpy(pData, bufferDesc.mData, bufferDesc.mElementDataNum * bufferDesc.mElementDataSize);
	//}


	return bufferResource;
}

bool D3DGRM::D3DGpuResourceManager::UploadBufferData(Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer, void* data, size_t elementSize, size_t elementNum, size_t bufferOffset)
{
	D3D12_RANGE readRange = { 0,0 };//읽기는 수행하지않는다
	void* pMappedData = nullptr;
	HRESULT hr = uploadBuffer->Map(0, &readRange, &pMappedData);
	if (FAILED(hr))
	{
		//log
		return false;
	}

	memcpy(&((char*)(pMappedData))[bufferOffset], data, elementSize * elementNum);
	uploadBuffer->Unmap(0, nullptr);

	return true;
}




void D3DGRM::D3DGpuResourceManager::ReleaseDescriptorHandle(ED3DResourceDescriptorType type, D3DDescriptorHandle handle)
{
	switch (type)
	{
	case ED3DResourceDescriptorType::eCBV:
	case ED3DResourceDescriptorType::eSRV:
	case ED3DResourceDescriptorType::eUAV:
		mCSUHeapManager->Free(handle);

		break;

	case ED3DResourceDescriptorType::eDSV:
		mDSVHeapManager->Free(handle);
		break;

	case ED3DResourceDescriptorType::eRTV:

		mRTVHeapManager->Free(handle);
		break;

	case ED3DResourceDescriptorType::eSMP:

		mSMPHeapManager->Free(handle);

	}

}







D3DGRM::D3DGpuDescriptorHeapManager* D3DGRM::D3DGpuResourceManager::GetCSUHeapManager() const
{
	return mCSUHeapManager.get();
}





D3DGRM::D3DGpuDescriptorHeapManager* D3DGRM::D3DGpuResourceManager::GetSMPHeapManager() const
{
	return mSMPHeapManager.get();
}





void D3DGRM::D3DGpuResourceManager::TransitionResourceState(GRM::IGpuResource* resource, D3D12_RESOURCE_STATES afterState)
{
	if (!resource)
		return;


	D3DGpuResource* d3dResource = static_cast<D3DGpuResource*>(resource);
	
	D3D12_RESOURCE_STATES beforeState = d3dResource->GetResourceState();

	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(d3dResource->GetResource().Get(), beforeState,
		afterState);
	
	mCommandList->Reset(mCommandAllocator.Get(),nullptr);

	mCommandList->ResourceBarrier(1, &transition);
	

	mCommandList->Close();
	ID3D12CommandList* commandList[] = { mCommandList.Get()};
	mCommandQueue->ExecuteCommandLists(1, commandList);

	FlushCommandQueue();

	d3dResource->SetResourceState(afterState);
	return;
}

void D3DGRM::D3DGpuResourceManager::Release(GRM::IGpuResource* resource)
{
	//보관

	if (resource == nullptr)
		return;

	mGarbageVector.push_back(static_cast<D3DGpuResource*>(resource));
}

void D3DGRM::D3DGpuResourceManager::FlushGarbageCollect()
{
	//Releas함수를통해 보관한 resource들을 
	//해제한다
	//올바르게 descriptor도 pool(descriptorManager)에 반납한다.



	//지금은 new 할당했기에 delete호출, 
	//후에 커스텀할당자를 사용하니, 수정필요

	for (auto resource : mGarbageVector)
	{

		for (auto descriptorHandle : resource->mDescriptorHandleTable)
		{
			ReleaseDescriptorHandle(descriptorHandle.first, descriptorHandle.second);
		}

		delete resource;
	}

}


