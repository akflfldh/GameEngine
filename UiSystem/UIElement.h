#pragma once

#include "ReflectSystem/ReflectionMacro.h"

#include "UiSystem/IUIComponent.h"
#include "UiSystem/UIComponentFactory.h"
#include "UiSystem/UIRectTransform.h"
#include "UiSystem/UISystemDllMacro.h"
#include "UiSystem/UIType.h"
#include <memory>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "UiSystem/Generated/UIElement.generated.h"

namespace UI
{
class UIRectTransform;
class UICanvas;

class UISYSTEM_API REFLECT_CLASS UIElement
{
    GENERATED_BODY(UIElement)

    friend class UIManager;
    friend class UIComponentFactory;
    friend class UICanvas;

  public:
    UIElement();
    UIElement(UIElementID id, const std::string &name);
    virtual ~UIElement();

    virtual void Begin();
    virtual void Update(float deltaTime);

    UIElementID GetID() const;

    UIElement *GetParent() const;

    void SetParent(UIElement *parent);

    void SetActiveFlag(bool flag);
    bool GetActiveFlag() const;

    const std::vector<UIElement *> GetChildVector() const;
    // 아에 Destroy메서드
    //
    UIRectTransform mTransform;

    void AddChildInternal(UIElement *child);
    void RemoveChildInternal(UIElement *child); // 단순히 child목록에서 뺴기만한다.
    void SetCanvasInternal(UICanvas *canvas);

    template <typename T> T *CreateUIComponent(const char *name);
    void ReleaseUIComponent(const char *name);

    template <typename T> size_t GetComponentsNum() const;
    template <typename T> size_t GetComponents(IUIComponent **comArray, size_t maxCount);

    // x,y 위치가 해당 ui영역 내부에 있는지 판정한다.
    bool IsPointInside(int x, int y);

    // 들어온 mouse 이벤트에따라 업데이트수행
    void UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext, bool &oCaptureActiveRequestFlag,
                               bool &oCaptureReleaseRequestFlag);
    uint32_t GetDepthValue() const;

    UICanvas *GetDestCanvas() const;

  private:
    size_t GetComponentsNum(const char *className) const;
    size_t GetComponentsInner(IUIComponent **comArray, size_t maxCount, const char *className);

    // 해당 X,Y좌표를 영역에 포함하는 컴포넌트를 리턴한다(가장 오버레이가높은)
    IUIComponent *InPointInside(int x, int y);

    // 캡처상태가아닌 hover상태에서의 업데이트
    void UpdateOnHover(const UIManagerMouseInputContext &mouseInputContext, bool &captureActiveRequestFlag,
                       bool &oCaptureReleaseRequestFlag);

    // 캡처상태일떄의 업데이트
    void UpdateOnCapture(const UIManagerMouseInputContext &mouseInputContext, bool &captureActiveRequestFlag,
                         bool &oCaptureReleaseRequestFlag);

    void SetDepthValue(uint32_t depth);

  private:
    std::string mName;
    UIElementID mID;

    //  std::vector<IUIComponent *> mComponentContainer;
    std::unordered_map<const char *, IUIComponent *> mComponentContainer;

    UIElement *mParent;

    // 비활성화되면 자식들도 모두 간접적으로 비활성화된다.
    bool mActiveFlag;

    // 향후 c스타일 이나 abi,crt문제가없도록 수정할것
    std::vector<UIElement *> mChildVector;

    UICanvas *mDestCanvas;

    UIElementInputStateContext mMouseInputStateContext;
    // 깊이값(ui요소들사이에서 깊이값이 작을수록 위에올라오는 ui가 된다. 최소는 1)
    uint32_t mDepthValue;
    REFLECT_PROPERTY
    int A = 2;
    REFLECT_PROPERTY
    int b = 2;
};

template <typename T> inline T *UIElement::CreateUIComponent(const char *name)
{
    return static_cast<T *>(UIComponentFactory::GetInstance()->Create(this, T::GetStaticClassName(), name));
}

template <typename T> inline size_t UIElement::GetComponentsNum() const
{

    return GetComponentsNum(T::GetStaticClassName());
}

template <typename T> inline size_t UIElement::GetComponents(IUIComponent **comArray, size_t maxCount)
{

    return GetComponentsInner(comArray, maxCount, T::GetStaticClassName());
}

} // namespace UI