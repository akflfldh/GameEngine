#pragma once

#include <string>
#include <unordered_map>

#include "UiSystem/UICanvas.h"
#include "UiSystem/UISystemDllMacro.h"
#include "UiSystem/UIType.h"

#include "InputSystem/InputType.h"

namespace UI
{
class UIElement;
class UISYSTEM_API UIManager
{
  public:
    static UIManager *GetInstance();
    ~UIManager();

    // 캔버스생성
    UICanvasID CreateCanvas(const std::string &name, ECanvasSizeMode sizeMode);
    // 캔버스를 파괴
    void DestroyCanvas(UICanvasID id);
    UICanvas *GetCanvas(UICanvasID id) const;

    // UIElement생성
    // UIElement *CreateUIElement(const std::string &name);
    //-> 타입별로 처리하도록한다. 내부적으로 리플렉션시스템을 사용할거고 (base of 로 uiElement의
    // 파생인지확인하거나할거다)

    // UIElement파괴
    void DestoryUIElement(UIElementID uiElementID);
    void DestoryUIElement(const char *uiElementName);

    UIElement *GetUIElement(UIElementID uiElementID) const;

    // Manager에서 직접호출하거나, UIElement,UICanvas에서 호출하면 간접적으로 호출된다.
    void AddUIElement(UICanvasID canvasID, UIElementID uiElementID);
    void AddUIElement(UICanvas *canvas, UIElement *uiElementID);
    void SetParent(UIElement *parent, UIElement *child);

    const std::unordered_map<UICanvasID, UICanvas *> &GetUICanvasAll() const;

    // 새로운 입력에따라 manager와, uiElement들의 상태를 업데이트한다.
    void UpdateInputState(const Quad::MouseContext &mouseContext, UICanvas **canvasList, size_t canvasNum);

    bool IsMouseCaptureOn() const;

    UIElement *CreateUIElement(const char *uiElementClassName, const char *instanceName);
    void DestoryUIElement(UIElement *element);

  private:
    UIManager();
    // 들어온 입력상태에 대해서 활성화된 ui들에대해 평가수행
    void EvaulateInputState(const Quad::MouseContext &mouseContext, UICanvas **canvasList, size_t canvasNum);
    // 평가후 설정된 타켓 ui들에게 이벤트전달
    void SendInputEventToTargetElements(const Quad::MouseContext &mouseContext);

  private:
    std::unordered_map<UICanvasID, UICanvas *> mUICanvasTable;

    std::unordered_map<UIElementID, UIElement *> mUIElementTable;
    std::unordered_map<std::string, UIElement *> mUIElementNameTable;

    UICanvasID mNextUICanvasID;
    std::vector<UICanvasID> mFreeCanvasIDVector;

    UIElementID mNextUIElementID;
    std::vector<UIElementID> mFreeUIElementIDVector;

    UIManagerInputStateContext mInputStateContext;
};

} // namespace UI
