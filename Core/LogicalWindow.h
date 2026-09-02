#pragma once

#include <Core/CoreDllExport.h>
#include <Core/CoreType.h>
#include <Core/ObjectPtr.h>
#include <Core/ViewportController.h>
#include <RenderSystem/RenderType.h>
#include <UiSystem/IViewContextInterface.h>
#include <UiSystem/UIType.h>

#include <CoreBase/CallbackSystem.h>
#include <vector>

class ObjectController;
class World;
class Map;

namespace UI
{
class UICanvas;
}
namespace Quad
{
struct RawInputData;
}

using OnMouseEnterCallbackSystem = Core::MultiCallbackSystem<>;
using OnMouseLeaveCallbackSystem = Core::MultiCallbackSystem<>;

namespace Core
{
// class CameraComponent;

class SuperController;

// 논리적 윈도우 ( 실제 윈도우(controller)에 포함되는)
// viewport, renderchannel을 가진다.
// 또한 ui canvas 를 유지한다.

struct WindowRenderConfig
{
    // 오버레이 윈도우인가? (최종 합성 단계, Clear 안함)
    bool bIsOverlay = false;

    // 렌더 타겟을 지울 것인가? (Overlay라면 false)
    bool bClearRenderTarget = true;

    float mBackBufferClearColor[4];

    bool bDebugGrid = false;

    // 렌더링 결과를 어디에 쓸 것인가? ("TempBackBuffer" or "BackBuffer")
    //  std::string OutputTargetName = "TempBackBuffer";

    // 렌더링 순서 (낮을수록 먼저 그려짐, Overlay는 가장 높게)
    // int RenderPriority = 0;
};

class CORE_API_LIB LogicalWindow : public UI::IViewContextInterface
{
  public:
    LogicalWindow();
    ~LogicalWindow();

    void Begin();

    void SetOwnerController(SuperController *superController);
    void *GetWindowHandle() const;

    void SetWorld(World *world);
    World *GetWorld() const;

    ::Map *GetMap() const;

    void SetActiveCanvas(UI::UICanvas *uiCanvas);
    UI::UICanvasID GetActiveCanvasID() const;
    UI::UICanvas *GetActiveCanvas() const;

    ViewportController mViewportController;

    // 3DWorldViewport는 mViewportController에 상대적이다. 따라서 screen 처리시 mViewportController의 leftTop의 값들을
    // offset으로 사용
    ViewportController m3DWorldViewportController;

    LogicalWindowInputContext mLogicalWindowInputContext;

    void OnResizeWindow(uint32_t width, uint32_t height);

    Render::Viewport GetGlobalSceneViewport() const;
    GlobalFrameData GetGlobalFrameData() const;
    GlobalFrameData GetUIGlobalFrameData() const;
    void UpdateFrameData();

    // 입력이벤트 발생시 호출된다.
    void OnInputEvent(const Quad::RawInputData &inputData);

    void SetActiveState(bool state);
    bool IsActive() const;

    void SetObjectController(ObjectController *objectController);
    ObjectController *GetObjectController() const;

    void SetBackBufferClearColor(float r, float g, float b, float a);
    const float *GetBackBufferClearColor() const;

    void OnNotifyReleasedKeyboardCapture();

#pragma region IViewContext
    // UI System
    glm::vec2 ConverToWorldPos(int clientPosX, int clientPosY) const override;

#pragma endregion

    SuperController *GetOwnerController();

    // 해당 윈도우의 Canvas의 UI들과 충돌하는지 여부
    // 전역 오버레이 LogicalWindow가 사용
    bool HitTestUI(int clientPosX, int clientPosY);

    const WindowRenderConfig &GetRenderConfig() const;
    void SetRenderConfig(const WindowRenderConfig &config);

    void SetDebugGridRender(bool state);

    // OnMouseEnter가 호출시 호출될 콜백시스템
    OnMouseEnterCallbackSystem mOnMouseEnterCallbackSystem;

    // 마우스가 해당 윈도우에 진입시 호출
    void OnMouseEnter();

    // OnMouseLeave가 호출시 호출될 콜백시스템
    OnMouseLeaveCallbackSystem mOnMouseLeaveCallbackSystem;

    // 마우스가 해당윈도우에서 이탈시 호출
    void OnMouseLeave();

    const CoreMath::Ray &GetWorldRay() const;
    CoreMath::Ray CaculateWorldRay(int clientPosX, int clientPosY) const;

  private:
    SuperController *mSuperController;

    // CurrActiveCanvas
    // 캔버스는 논리적윈도우마다 그냥 하나만 참조하면될듯 , 필요하면 참조를 바꾸면되고
    World *mWorld;
    UI::UICanvas *mCurrActiveCanvas;

    // 설정되어있다면 LogicalWindow가 활성화될때 인풋시스템에 독점하도록 설정할것이다
    // 일반적으로 CameraController가 될것이다 일단 에디터에서는
    ObjectPtr<ObjectController> mObjectController;

    GlobalFrameData mGlobalFrameData;
    GlobalFrameData mUIGlobalFrameData;

    // 창이 마우스클릭을 받아서 활성화 된상태인지여부
    bool mActiveState;

    float mBackBufferClearColor[4];

    WindowRenderConfig mRenderConfig;

    int mCameraComponentIndex = 0;

    CoreMath::Ray mWorldRay;
};

} // namespace Core