#include "D3DRenderSystem.h"

#include"D3DRenderChannelSystem.h"

#include"D3DWindowRenderManager.h"
#include"D3DRenderPassManager.h"
#include"D3DWindowRenderData.h"
#include<D3DGpuTexture.h>
#include<D3DGpuResourceManager.h>
#include"D3DMaterialManager.h"


#include<d3dx12.h>


#include"RenderSystemInternalHelper.h"

#include<algorithm>

D3DRender::D3DRenderSystem::D3DRenderSystem(Microsoft::WRL::ComPtr<IDXGIFactory> factory, Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, D3DGRM::D3DGpuResourceManager* gpuResourceManager)
	:mFactory(factory),mDevice(device),mCommandQueue(commandQueue), mNextChannelID(0),mGpuResourceManager(gpuResourceManager)
{
	mWindowRenderManager = std::make_unique< D3DWindowRenderManager>(mDevice, mFactory);
	mMaterialManager = std::make_shared<D3DMaterialManager>(device, gpuResourceManager);
	Render::IMaterialManager::SetMaterialManagerImpl(mMaterialManager.get());
	


	mRenderPassManager = std::make_unique<D3DRenderPassManager>(device, gpuResourceManager,mMaterialManager);
	


	HRESULT result =S_OK;
	result =mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
	result =mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mCommandList));
	


}

D3DRender::D3DRenderSystem::~D3DRenderSystem()
{
}

Render::RenderChannelID D3DRender::D3DRenderSystem::RegisterRenderChannel(const Render::CreationRenderChannelInfo& creationChannelInfo)
{


	bool result = mWindowRenderManager->RegisterWindow(creationChannelInfo);
	if (result == false)
		return -1;


	std::shared_ptr<D3DWindowRenderData>windowRenderData = mWindowRenderManager->GetWindowRenderData(creationChannelInfo.mWindowHandle);


	mRenderChannelSystemTable[mNextChannelID]= std::make_unique<D3DRenderChannelSystem>(creationChannelInfo,mDevice, mCommandQueue,mGpuResourceManager, mRenderPassManager->CreateDefaultRenderPipeline(windowRenderData));

	mChannelWindowMapTable[creationChannelInfo.mWindowHandle].push_back(mNextChannelID);

	mNextChannelID++;
	return 0;
}

void D3DRender::D3DRenderSystem::StartWindow(Render::WindowHandle windowHandle)
{

	FlushCommandQueue(windowHandle);

	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), nullptr);



	//window curr back buffer를 렌더타켓상태로 전환
	std::shared_ptr<D3DWindowRenderData> windowRenderData = mWindowRenderManager->GetWindowRenderData(windowHandle);
	int currentBackBufferIndex = windowRenderData->GetCurrentBackBufferIndex();


	D3DGRM::D3DGpuTexture* backBuffer =static_cast<D3DGRM::D3DGpuTexture*>(windowRenderData->GetBackBuffer(currentBackBufferIndex).getResource());
	D3DGRM::D3DGpuTexture* depthStencilBuffer =static_cast<D3DGRM::D3DGpuTexture*>(windowRenderData->GetDepthStencilBuffer().getResource());
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = windowRenderData->GetSwapChain();
	D3DGRM::D3DDescriptorHandle backBufferDescriptorHandle;
	D3DGRM::D3DDescriptorHandle depthStecnilDescriptorHandle;

	if (depthStencilBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eDSV, depthStecnilDescriptorHandle))
	{
		//StartWindow함수로 정보를 받아 설정할수있도록 확장하자 
		mCommandList->ClearDepthStencilView(depthStecnilDescriptorHandle.mCpuDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH |
			D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	}



	if (backBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, backBufferDescriptorHandle))
	{


		auto backBufferPresentTransition = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer->GetResource().Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		mCommandList->ResourceBarrier(1, &backBufferPresentTransition);



		float color[4] = { 1,0,0,0 };
		mCommandList->ClearRenderTargetView(backBufferDescriptorHandle.mCpuDescriptorHandle, color, 0, nullptr);

		mCommandList->Close();
		ID3D12CommandList* commandLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(1, commandLists);

	}







}

void D3DRender::D3DRenderSystem::BeginFrame(Render::RenderChannelID channelID, const Render::FrameContext& frameContext)
{
	auto it = mRenderChannelSystemTable.find(channelID);

	if (it == mRenderChannelSystemTable.end())
		return;


	(*it).second->BeginFrame(frameContext);

}

void D3DRender::D3DRenderSystem::SubmitRenderItems(Render::RenderChannelID channelID, const std::vector<Render::RenderItem>& renderItemVector)
{
	auto it = mRenderChannelSystemTable.find(channelID);
	if (it == mRenderChannelSystemTable.end())
		return;


	//정렬

	std::vector<Render::InternalRenderItem> internalRenderItemVector;
	internalRenderItemVector.reserve(renderItemVector.size());
	for (size_t i = 0; i < renderItemVector.size(); ++i)
	{
		internalRenderItemVector.push_back(ConvertInteranlRenderItem(renderItemVector[i]));
	}

	//그후 렌더패스들에게는 이렇게 정렬된 렌더ITEM들이제공된다
	std::sort(internalRenderItemVector.begin(), internalRenderItemVector.end());

	(*it).second->SubmitRenderItem(std::move(internalRenderItemVector));


}

//void D3DRender::D3DRenderSystem::SubmitRenderItems(Render::RenderChannelID channelID, std::vector<Render::RenderItem>&& renderItemVector)
//{
//
//	auto it = mRenderChannelSystemTable.find(channelID);
//	if (it == mRenderChannelSystemTable.end())
//		return;
//
//	(*it).second->SubmitRenderItem(std::move(renderItemVector));
//
//}




void D3DRender::D3DRenderSystem::EndFrame(Render::RenderChannelID channelID)
{
	auto it = mRenderChannelSystemTable.find(channelID);
	if (it == mRenderChannelSystemTable.end())
		return;

	(*it).second->EndFrame();


}

void D3DRender::D3DRenderSystem::PresentWindow(Render::WindowHandle windowHandle)
{
	std::shared_ptr<D3DWindowRenderData> windowRenderData = mWindowRenderManager->GetWindowRenderData(windowHandle);
	int currentBackBufferIndex = windowRenderData->GetCurrentBackBufferIndex();



	D3DGRM::D3DGpuTexture * backBuffer = static_cast<D3DGRM::D3DGpuTexture*>(windowRenderData->GetBackBuffer(currentBackBufferIndex).getResource());
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = windowRenderData->GetSwapChain();
	D3DGRM::D3DDescriptorHandle backBufferDescriptorHandle;
	

	if (backBuffer->GetDescriptorHandle(D3DGRM::ED3DResourceDescriptorType::eRTV, backBufferDescriptorHandle))
	{
		mCommandList->Reset(mCommandAllocator.Get(), nullptr);


		auto backBufferPresentTransition = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer->GetResource().Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		mCommandList->ResourceBarrier(1, &backBufferPresentTransition);


		mCommandList->Close();
		ID3D12CommandList* commandLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(1, commandLists);

		swapChain->Present(0, 0);
		windowRenderData->IncrementBackBufferIndex();

	}



}

void D3DRender::D3DRenderSystem::FlushCommandQueue(Render::WindowHandle windowHandle)
{
	std::shared_ptr<D3DWindowRenderData> windowRenderData = mWindowRenderManager->GetWindowRenderData(windowHandle);

	windowRenderData->IncrementFenceValue();

	Microsoft::WRL::ComPtr<ID3D12Fence>  fence = windowRenderData->GetFence();

	size_t currentFenceValue = windowRenderData->GetCurrentFenceValue();
	mCommandQueue->Signal(fence.Get(), currentFenceValue);

	if (fence->GetCompletedValue() < currentFenceValue)
	{
		HANDLE event = windowRenderData->GetFenceEventHandle();
		fence->SetEventOnCompletion(currentFenceValue, event);

		WaitForSingleObject(event, INFINITE);

	}



}

Render::InternalRenderItem D3DRender::D3DRenderSystem::ConvertInteranlRenderItem(const Render::RenderItem& renderItem) const
{

	Render::InternalRenderItem internalRenderItem;
	internalRenderItem.mScissor = renderItem.mScissor;
	internalRenderItem.mMaterialID = renderItem.mMaterialID;
	internalRenderItem.mInstance = renderItem.mInstance;
	internalRenderItem.mMeshItem = renderItem.mMeshItem;


	internalRenderItem.mFirstSortKey = Render::GetRenderItemFirstSortKey(renderItem.mScissor, renderItem.mMaterialID);
	internalRenderItem.mSecondSortKey = Render::GetRenderItemSecondSortKey(renderItem.mMeshItem, renderItem.mInstance);



	return internalRenderItem;
}
