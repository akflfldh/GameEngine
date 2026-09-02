#pragma once

#include <UiSystem/UISystemDllMacro.h>

#include <UiSystem/UIType.h>
#include <vector>
namespace UI
{
class UICanvas;
class UIRenderableComponent;
struct UIRenderProxy
{
    // glm::mat4 mTransform;
    UI::UICanvas *mCanvas;
    UIRenderableComponent *mRenderableComponent;

    // TODO
    /*
        proxy에 vertex, index등 렌더링을 위한 모든것을 복사하자,그리고 RenderableCom은 제거하자
        그래야 render 로직과  상위 로직에대해서 멀티스레드가 가능해진다.
    */

    // std::vector<UI::UIVertex> mVertexList;
    // std::vector<uint32_t> mIndexList;
    // CoreAsset::Material *mUIMaterial;
};

class UISYSTEM_API IUIRenderProxyManager
{
  public:
    static IUIRenderProxyManager *GetInstance();

    // GetInstance()/호출전 가장먼저 호출할것
    static void SetRenderProxyManager(IUIRenderProxyManager *renderProxyManager);

    virtual void RegisterRenderProxy(UIRenderProxy *renderProxy) = 0;
    virtual void UnRegisterRenderProxy(UIRenderProxy *renderProxy) = 0;

  private:
    static IUIRenderProxyManager *mRenderProxyManager;
};

} // namespace UI