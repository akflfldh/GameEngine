#include "pch.h"
#include "D3DSystemInitializer.h"


#include<D3DGpuResourceManager.h>
#include<D3DRenderSystem.h>
#include<D3DMaterialManager.h>

D3DSystemInitializer::D3DSystemInitializer::D3DSystemInitializer()
{
}

void D3DSystemInitializer::D3DSystemInitializer::Initialize()
{

#ifdef _DEBUG
	OutputDebugString(L"Before debugController declaration\n");

	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;

	OutputDebugString(L"After debugController declaration\n");
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
	
#endif
	HRESULT result = CreateDXGIFactory(IID_PPV_ARGS(&mFactory));
	
	result = mFactory->EnumAdapters(0, &mAdapter);
	

	result = D3D12CreateDevice(mAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mDevice));


	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.NodeMask = 0;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	result =mDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&mCommandQueue));

	mD3DGpuResourceManager = new D3DGRM::D3DGpuResourceManager(mDevice, mCommandQueue);
	GRM::IGpuResourceManager::SetGpuResourceManagerImpl(mD3DGpuResourceManager);


	mD3DRenderSystem = new D3DRender::D3DRenderSystem(mFactory, mDevice, mCommandQueue,mD3DGpuResourceManager);
	Render::IRenderSystem::SetRenderSystemImpl(mD3DRenderSystem);


	
	//materialManager는 렌더시스템내부에서 생성초기화된다
	/*mMaterialManager = new D3DRender::D3DMaterialManager(mDevice, mD3DGpuResourceManager);
	Render::IMaterialManager::SetMaterialManagerImpl(mMaterialManager);*/

}
