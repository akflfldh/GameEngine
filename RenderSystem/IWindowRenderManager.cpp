#include "IWindowRenderManager.h"
#include "D3DWindowRenderManager.h"
Render::IWindowRenderManager *Render::IWindowRenderManager::GetInstance()
{
#ifdef D3DX
    D3DRender::D3DWindowRenderManager *windowRenderManager = D3DRender::D3DWindowRenderManager::GetInstance();
    return windowRenderManager;
#endif
}

Render::IWindowRenderManager::IWindowRenderManager() {}

Render::IWindowRenderManager::~IWindowRenderManager() {}
