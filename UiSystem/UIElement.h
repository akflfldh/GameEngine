#pragma once

#include "ReflectSystem/ReflectionMacro.h"

#include "UiSystem/IUIComponent.h"
#include "UiSystem/UIComponentFactory.h"
#include "UiSystem/UIRectTransform.h"
#include "UiSystem/UISystemDllMacro.h"
#include "UiSystem/UIType.h"
#include <CoreBase/BaseClass.h>
#include <CoreBase/CallbackSystem.h>
#include <UiSystem/UIElementTyprDef.h>
#include <memory>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "UIElement.generated.h"

namespace UI
{
class UIElement;
}

using OnLostKeyboardFocusCallbackSystem = Core::MultiCallbackSystem<>;
using OnSetKeyboardFocusCallbackSystem = Core::MultiCallbackSystem<>;
using OnLostMouseFocusCallbackSystem = Core::MultiCallbackSystem<>;
using OnSetMouseFocusCallbackSystem = Core::MultiCallbackSystem<>;

using OnAddedChildElementCallbackSystem = Core::MultiCallbackSystem<UI::UIElement *>;
using OnRemovedChildElementCallbackSystem = Core::MultiCallbackSystem<UI::UIElement *>;

using OnActiveElementCallbackSystme = Core::MultiCallbackSystem<bool>;

namespace UI
{

class UIElement;

UISYSTEM_API UIElement *CreateUIElement(UI::UIElement *parent, const char *staticClassName, const char *instanceName);

class UIRectTransform;
class UICanvas;

class UISYSTEM_API REFLECT_CLASS(EngineClass) UIElement : public BaseClass
{
    GENERATED_BODY(UIElement)

    friend class UIManager;
    friend class UIComponentFactory;
    friend class UICanvas;

  public:
    UIElement();
    UIElement(UIElementID id, const std::string &name);
    virtual ~UIElement();

    virtual void Serialize(Arch &arch) override;

    void Begin();
    virtual void Update(float deltaTime);

    const std::string &GetName() const;
    UIElementID GetID() const;

    void Destroy();
    UIElement *GetParent() const;

    void OnWindowResize(float w, float h);

    void SetParent(UIElement *parent);

    void SetActiveFlag(bool flag);
    bool GetActiveFlag() const;

    void SetOnlyVisible(bool flag);
    bool GetOnlyVisible() const;

    void SetRenderLayer(EUIRenderLayer layer);
    EUIRenderLayer GetRenderLayer() const;

    const std::vector<UIElement *> &GetChildVector() const;
    // 아에 Destroy메서드
    //
    UIRectTransform mTransform;

    SRECT GetScissorRectRegion() const;

    void AddChildInternal(UIElement *child);
    void RemoveChildInternal(UIElement *child); // 단순히 child목록에서 뺴기만한다.
    void SetCanvasInternal(UICanvas *canvas);

    template <typename T> T *CreateChildUIElement(const char *instanceName);

    template <typename T> T *CreateUIComponent(const char *name);
    void ReleaseUIComponent(const char *name);
    void ReleaseUIComponent(IUIComponent *com);

    template <typename T> size_t GetComponentsNum() const;
    template <typename T> size_t GetComponents(T **comArray, size_t maxCount);

    // x,y 위치가 해당 ui영역 내부에 있는지 판정한다.
    // 입력테스트라면  내부적으로 InputEnable한 com들만 판정수행
    bool IsPointInside(int x, int y, bool bInputTest = false);

    // 들어온 mouse 이벤트에따라 업데이트수행
    void UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext, bool &oCaptureActiveRequestFlag,
                               bool &oCaptureReleaseRequestFlag);

    void SetDepthValue(uint32_t depth);
    uint32_t GetDepthValue() const;

    UICanvas *GetDestCanvas() const;

    IUIComponent *GetComponent(const char *instacneName) const;

    UIElementHandle GetHandle() const;

#pragma region InputEvent
    virtual void OnHover(int x, int y); // hover 될때 호출됨
    virtual void OnReleaseHover();      // hover에서 빠져나갈때 호출됨

    virtual void HandleInput(const Quad::RawInputData &inputData);

    virtual void OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY);
    virtual void OnMouseClick(const Quad::RawInputData &inputData);
    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY);
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY);
    virtual void OnMouseWheel(const Quad::RawInputData &inputData, float worldPosX, float worldPosY, bool &bConsume);

    virtual void OnKeyDown(const Quad::EKeyCode &key);
    virtual void OnKeyUp(const Quad::EKeyCode &key);
    virtual void OnChar(const uint32_t keychar);

#pragma endregion

    // OnHover,OnMouseMove 에서 호출하여 적절한 com의 입력메서드 호출
    void UpdateComponentMouseState(const Quad::RawInputData &inputData, int x, int y);

    // 마우스 캡처
    void RequestMouseCaptureInput(IUIComponent *component);
    void ReleaseMouseCaptureInput();

    // 키보드 캡처
    void RequestKeyboardCaptureInput(IUIComponent *component);
    void ReleaseKeyboardCaptureInput();

    void OnSetMouseFocus();
    void OnLostMouseFocus();

    void OnSetKeyboardFocus();
    void OnLostKeyboardFocus();

    bool GetUseScissorRect() const;
    void SetUseScissorRect(bool flag);

    OnLostKeyboardFocusCallbackSystem mOnLostKeyboardFocusCallbackSystem;
    OnSetKeyboardFocusCallbackSystem mOnSetKeyboardFocusCallbackSystem;
    OnLostMouseFocusCallbackSystem mOnLostMouseFocusCallbackSystem;
    OnSetMouseFocusCallbackSystem mOnSetMouseFocusCallbackSystem;

    bool GetDeadState() const;

#pragma region Transform

    void SetPositionLocal(const glm::vec2 &pos);
    void SetPositionLocal(float x, float y);
    void SetPositionWorld(const glm ::vec2 &pos);
    void SetPositionWorld(float x, float y);

    void TranslateLocal(const glm::vec2 &shift);

    virtual void SetSize(const glm::vec2 &size);
    virtual void SetSize(float w, float h);
    virtual void SetHeight(float h);
    virtual void SetWidth(float w);

    int GetWidth() const;
    int GetHeight() const;
    CoreMath::Vector2 GetSize() const;

    void NotifyTransformChanged(ETransformChangeType type);
    virtual void OnTransformChanged(ETransformChangeType type) {};

    void BroadCastChangedSize();
    Core::MultiCallbackSystem<UI::UIElement *> mOnChangedSizeCallbackSystem;

    void SetPosPivotActive(bool flag);
    bool GetPosPivotActive() const;

    void SetHorizontalPivotSide(EUIPosPivotHorizontal pivotSide);
    void SetVerticalPivotSide(EUIPosPivotVertical pivotSide);

    void SetHorizontalPivotOffset(float offset);
    void SetVerticalPivotOffset(float offset);

    void OnParentSizeChanged();
    void UpdatePosPivot();
    void ApplyPosPivotInParent(const CoreMath::Vector2 &parentSize);
#pragma endregion

    OnAddedChildElementCallbackSystem mOnAddedChildElementCallbackSystem;
    OnRemovedChildElementCallbackSystem mOnRemovedChildElementCallbackSystem;

    OnActiveElementCallbackSystme mOnActiveElementCallbackSystem;

    UIElement *GetKeyboardCaptureScope() const;
    void SetKeyboardCaptureScope(UIElement *scope);

  protected:
    bool mIsBegun;
    virtual void OnBegin() {};
    virtual bool IsPointInsideDefault(float x, float y) const;

  private:
    size_t GetComponentsNum(const char *className) const;
    size_t GetComponentsInner(IUIComponent **comArray, size_t maxCount, const char *className);

    // 해당 X,Y좌표를 영역에 포함하는 컴포넌트를 리턴한다(가장 오버레이가높은)
    IUIComponent *InPointInside(int x, int y);
    //// 캡처상태가아닌 hover상태에서의 업데이트
    // void UpdateOnHover(const UIManagerMouseInputContext &mouseInputContext, bool &captureActiveRequestFlag,
    //                    bool &oCaptureReleaseRequestFlag);

    //// 캡처상태일떄의 업데이트
    // void UpdateOnCapture(const UIManagerMouseInputContext &mouseInputContext, bool &captureActiveRequestFlag,
    //                      bool &oCaptureReleaseRequestFlag);

    // factory가 사용하는 api
    void RegisterComponent(const char *instacneName, IUIComponent *component);
    void UnRegisterComponent(IUIComponent *component);

    // component가 추가,제거 되고 나서 호출(항상 깊이값을 기준으로 정렬되어야, com에서 이벤트 전달받는순서가
    // 보장된다.)
    void SortComponents();

    // Manager가 사용
    void SetUIElementHandle(const UIElementHandle &handle);

    void OnChangedChildDepthValue();

    void SetDeadState();

  private:
    std::string mName;
    UIElementID mID;

    std::vector<IUIComponent *> mComponentContainer;
    // std::unordered_map<const char *, IUIComponent *> mComponentContainer;

    IUIComponent *mMouseCapturedComponent;
    IUIComponent *mKeyboardCapturedComponent;

    UIElement *mParent;

    // 비활성화되면 자식들도 모두 간접적으로 비활성화된다.
    bool mActiveFlag;

    // 입력처리에대한 무효화여부
    bool mOnlyVisible;

    UIElementHandle mUIElementHandle;

    std::vector<UIElement *> mChildVector;

    UICanvas *mDestCanvas;

    UIElementInputStateContext mMouseInputStateContext;
    // 깊이값(ui요소들사이에서 깊이값이 작을수록 위에올라오는 ui가 된다. 최소는 1)
    uint32_t mDepthValue;
    bool mUseScissorRECT;
    bool mDeadState;

    bool mChangedSizeDirty = false;

    // 현재  Top ui들만 유효하다
    UIPosPivotContext mPosPviotContext;

    UIElement *mKeyboardCaptureScope = nullptr;

    EUIRenderLayer mRenderLayer = EUIRenderLayer::eNormal;
};

template <typename T> inline T *UIElement::CreateChildUIElement(const char *instanceName)
{
    return static_cast<T *>(CreateUIElement(this, T::GetStaticClassName(), instanceName));
}

template <typename T> inline T *UIElement::CreateUIComponent(const char *name)
{
    return static_cast<T *>(UIComponentFactory::GetInstance()->Create(this, T::GetStaticClassName(), name));
}

template <typename T> inline size_t UIElement::GetComponentsNum() const
{

    return GetComponentsNum(T::GetStaticClassName());
}

template <typename T> inline size_t UIElement::GetComponents(T **comArray, size_t maxCount)
{

    std::vector<IUIComponent *> tempArray(maxCount, nullptr);

    size_t num = GetComponentsInner(tempArray.data(), maxCount, T::GetStaticClassName());

    //  결과를 사용자의 배열로 안전하게 캐스팅하여 복사
    for (size_t i = 0; i < num; ++i)
    {
        // static_cast를 사용하여 포인터 오프셋 보정이 올바르게 수행됨
        comArray[i] = static_cast<T *>(tempArray[i]);
    }

    return num;
}

} // namespace UI