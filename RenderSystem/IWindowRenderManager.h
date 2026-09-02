#pragma once

#include <D3DGpuResourceManager/GRMPtr.h>
#include <RenderSystem/RenderSystemDllMacro.h>

namespace Render
{

class RENDER_SYSTEM_API IWindowRenderManager
{
  public:
    static IWindowRenderManager *GetInstance();

    virtual GRM::GRMPtr GetCurrentSwapchainBackBuffer(void *windowHandle) = 0;
    virtual GRM::GRMPtr GetSwapchainBackBuffer(void *windowHandle, int index) = 0;

    virtual GRM::GRMPtr GetDepthStencilBuffer(void *windowHandle) = 0;

  protected:
    IWindowRenderManager();
    virtual ~IWindowRenderManager() = 0;

  private:
};
} // namespace Render
