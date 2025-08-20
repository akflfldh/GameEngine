#pragma once

#define D3DX

#ifdef D3DX

#include "SystemInitializer/ISystemInitializer.h"
#include "SystemInitializer/SystemInitializerDll.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>

namespace D3DGRM
{
class D3DGpuResourceManager;
}
namespace D3DRender
{
class D3DRenderSystem;
class D3DMaterialManager;
} // namespace D3DRender

namespace D3DSystemInitializer
{

class SYSTEM_INITIALIZER_API D3DSystemInitializer : public SystemInitializer::ISystemInitializer
{
  public:
    D3DSystemInitializer();
    virtual ~D3DSystemInitializer() = default;

    virtual void Initialize() override;

  private:
    class D3DGRM::D3DGpuResourceManager *mD3DGpuResourceManager;
    class D3DRender::D3DRenderSystem *mD3DRenderSystem;
    //	class D3DRender::D3DMaterialManager* mMaterialManager;
};

} // namespace D3DSystemInitializer

#endif