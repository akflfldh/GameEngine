#include "D3DSystemInitializer.h"
#include "pch.h"

#include "SystemInitializer/D3DSystemInitializer.h"
#include <CoreDevice/D3DCoreDevice.h>
#include <D3DGpuResourceManager/D3DGpuResourceManager.h>
#include <RenderSystem/D3DMaterialManager.h>
#include <RenderSystem/D3DRenderSystem.h>
D3DSystemInitializer::D3DSystemInitializer::D3DSystemInitializer() {}

D3DSystemInitializer::D3DSystemInitializer::~D3DSystemInitializer() = default;

void D3DSystemInitializer::D3DSystemInitializer::Initialize()
{

    Core::D3DCoreDevice *coreDevice = static_cast<Core::D3DCoreDevice *>(Core::CoreDevice::GetInstance());

    mD3DGpuResourceManager = std::make_unique<D3DGRM::D3DGpuResourceManager>(coreDevice->mDevice);
    GRM::IGpuResourceManager::SetGpuResourceManagerImpl(mD3DGpuResourceManager.get());

    mD3DRenderSystem = std::make_unique<D3DRender::D3DRenderSystem>(
        coreDevice->mFactory, coreDevice->mDevice, coreDevice->mCommandQueue, mD3DGpuResourceManager.get());
    Render::IRenderSystem::SetRenderSystemImpl(mD3DRenderSystem.get());

    // materialManager는 렌더시스템내부에서 생성초기화된다
    /*mMaterialManager = new D3DRender::D3DMaterialManager(mDevice, mD3DGpuResourceManager);
    Render::IMaterialManager::SetMaterialManagerImpl(mMaterialManager);*/
}
