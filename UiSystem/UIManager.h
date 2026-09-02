#pragma once

#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "UiSystem/UICanvas.h"
#include "UiSystem/UISystemDllMacro.h"
#include "UiSystem/UIType.h"

#include <CoreBase/CallbackSystem.h>
#include <InputSystem/IInputHandler.h>
#include <UiSystem/IViewContextInterface.h>
#include <UiSystem/UIElementTyprDef.h>

#include "InputSystem/InputType.h"

using CaptureCallbackSystem = Core::MultiCallbackSystem<>;

namespace UI
{

class UIElement;

struct UIElementSlot
{
    uint64_t mGeneration;
    UIElement *mUIElement;
};

class UISYSTEM_API UIManager
{
  public:
    static UIManager *GetInstance();
    ~UIManager();

    void Begin();
    void BeginCanvas(UICanvasID canvasID);
    void Update(float deltaTime);
    void CleanUp();
    void EndFrame();

    // 캔버스생성
    UICanvasID CreateCanvas(const std::string &name, ECanvasSizeMode sizeMode);
    // 캔버스를 파괴
    void DestroyCanvas(UICanvasID id);
    UICanvas *GetCanvas(UICanvasID id) const;

    // cavnas 활성화를알려야 해당 canvas가 실제로 update된다.
    void ActivateCanvas(UICanvas *canvas);
    // 해당 Canvas에대한 RefCount가 0이되면 비활성화된다.
    void RequestDeActivateCanvas(UICanvas *canvas);

    // UIElement생성
    // UIElement *CreateUIElement(const std::string &name);
    //-> 타입별로 처리하도록한다. 내부적으로 리플렉션시스템을 사용할거고 (base of 로 uiElement의
    // 파생인지확인하거나할거다)

    // UIElement파괴
    void DestoryUIElement(UIElementID uiElementID);
    void DestoryUIElement(const char *uiElementName);

    UIElement *GetUIElement(UIElementID uiElementID) const;
    UIElement *GetUIElement(UIElementHandle uiElementHandle) const;

    // Manager에서 직접호출하거나, UIElement,UICanvas에서 호출하면 간접적으로 호출된다.
    void AddUIElement(UICanvasID canvasID, UIElementID uiElementID);
    void AddUIElement(UICanvas *canvas, UIElement *uiElementID);
    void SetParent(UIElement *parent, UIElement *child);

    const std::unordered_map<UICanvasID, UICanvas *> &GetUICanvasAll() const;

    // 새로운 입력에따라 manager와, uiElement들의 상태를 업데이트한다.
    void UpdateInputState(const Quad::MouseContext &mouseContext, const std::vector<UICanvas *> &canvasList,
                          size_t canvasNum);

    bool IsMouseCaptureOn() const;

    UIElement *CreateUIElement(const char *uiElementClassName, const char *instanceName);

    void DestoryUIElement(UIElement *element);

#pragma region InputHandle

    void HandleInput(const Quad::RawInputData &inputData, const IViewContextInterface &viewContext, UICanvas *canvas,
                     bool &bConsume);

    // uiElement들이 마우스 캡처요청
    void SetMouseCapture(UIElement *element);
    void ReleaseMouseCapture(UI::UIElement *element);

    // 키보드 캡처요청
    void SetKeyboardCapture(UIElement *element);
    void ReleaseKeyboardCapture();

    void ReleaseKeyboardCaptureProgrammatically();

    void DispatchMouseMoveEvent(UIElement *targetElement, int mouseWorldPosX, int mouseWorldPosY,
                                const Quad::RawInputData &inputData);
    void DispatchMouseButtonEvent(UIElement *targetElement, int mouseWorldPosX, int mouseWorldPosY,
                                  const Quad::RawInputData &inputData);
    void DispatchMouseWheelEvent(UIElement *targetElement, int mouseWorldPosX, int mouseWorldPosY,
                                 const Quad::RawInputData &inputData);

    void OnMouseLeaveCanvas(UICanvas *canvas);
    /// void OnMouseCaptureLost();
#pragma endregion

    CaptureCallbackSystem mMouseCaptureCallbackSystem;
    CaptureCallbackSystem mMouseReleaseCaptureCallbackSystem;

    CaptureCallbackSystem mKeyboardCaptureCallbackSystem;
    CaptureCallbackSystem mKeyboardReleaseCaptureCallbackSystem;

    bool HiTest(UICanvas *canvas, float worldPosX, float worldPosY);
    UI::UIElement *GetHittedElement(UICanvas *canvas, float worldPosX, float worldPosY);

  private:
    UIManager();
    // 들어온 입력상태에 대해서 활성화된 ui들에대해 평가수행
    void EvaulateInputState(const Quad::MouseContext &mouseContext, const std::vector<UICanvas *> &canvasList,
                            size_t canvasNum);

    // 평가후 설정된 타켓 ui들에게 이벤트전달
    void SendInputEventToTargetElements(const Quad::MouseContext &mouseContext);

    UIElement *EvaulateHoverElement(float mouseWorldPosX, float mouseWorldPosY, UICanvas *canvas);
    UIElement *EvaluateHoverElementRecursive(UIElement *element, float mouseWorldPosX, float mouseWorldPosY);

    UIElement *ResolveMouseInputTarget(UIElement *hovered) const;

    // UIElement가 생성될때 호출된다.
    void RegisterToSlotPool(UIElement *uiElement);
    void UnRegisterToSlotPool(uint64_t slotIndex);

    void DestroyDeadUIElement();

    bool IsSameOrDescendant(UIElement *target, UIElement *ancestor) const;

    void PushMouseInputScope(UIElement *root);
    void PopMouseInputScope(UIElement *root);
    UIElement *GetCurrentMouseInputScope() const;
    bool IsCurrentMouseInputScope(UIElement *element) const;

  private:
    std::unordered_map<UICanvasID, UICanvas *> mUICanvasTable;

    std::unordered_map<UIElementID, UIElement *> mUIElementTable;
    std::unordered_map<std::string, std::vector<UIElement *>> mUIElementNameTable;

    std::vector<UICanvas *> mActiveCanvasList;

    UICanvasID mNextUICanvasID;
    std::vector<UICanvasID> mFreeCanvasIDVector;

    UIElementID mNextUIElementID;
    std::vector<UIElementID> mFreeUIElementIDVector;

    UIManagerInputStateContext mInputStateContext;

    // TODO Canvas에서 만들어진 uiElement를 유지하지만 매니저는 전체를 유지해야한다 , 단 Slot으로
    std::vector<UIElementSlot> mUIElementSlotPool;
    std::queue<uint64_t> mUIElementFreeIndexPool;
    uint64_t mNextPoolIndex;

    std::vector<UIElement *> mDeadUIElementList;
};

} // namespace UI
