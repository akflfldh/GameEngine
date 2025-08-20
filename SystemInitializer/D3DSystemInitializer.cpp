#include "pch.h"

#include "SystemInitializer/D3DSystemInitializer.h"
#include <CoreDevice/D3DCoreDevice.h>
#include <D3DGpuResourceManager/D3DGpuResourceManager.h>
#include <RenderSystem/D3DMaterialManager.h>
#include <RenderSystem/D3DRenderSystem.h>
D3DSystemInitializer::D3DSystemInitializer::D3DSystemInitializer() {}

void D3DSystemInitializer::D3DSystemInitializer::Initialize()
{

#ifdef _DEBUG
    OutputDebugStringW(L"Before debugController declaration\n");

    Microsoft::WRL::ComPtr<ID3D12Debug> debugController;

    OutputDebugStringW(L"After debugController declaration\n");
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
    }

#endif

    Core::D3DCoreDevice *coreDevice = static_cast<Core::D3DCoreDevice *>(Core::CoreDevice::GetInstance());

    mD3DGpuResourceManager = new D3DGRM::D3DGpuResourceManager(coreDevice->mDevice, coreDevice->mCommandQueue);
    GRM::IGpuResourceManager::SetGpuResourceManagerImpl(mD3DGpuResourceManager);

    mD3DRenderSystem = new D3DRender::D3DRenderSystem(coreDevice->mFactory, coreDevice->mDevice,
                                                      coreDevice->mCommandQueue, mD3DGpuResourceManager);
    Render::IRenderSystem::SetRenderSystemImpl(mD3DRenderSystem);

    // materialManager는 렌더시스템내부에서 생성초기화된다
    /*mMaterialManager = new D3DRender::D3DMaterialManager(mDevice, mD3DGpuResourceManager);
    Render::IMaterialManager::SetMaterialManagerImpl(mMaterialManager);*/
}
