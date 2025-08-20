#pragma once

#include <ReflectSystem/ReflectionMacro.h>

#include "UiSystem/UIType.h"

#include "UiSystem/Generated/IUIComponent.generated.h"

namespace UI
{
class UIElement;

class UISYSTEM_API REFLECT_CLASS IUIComponent
{
    friend class UIComponentFactory;

    GENERATED_BODY(IUIComponent)

  public:
    IUIComponent();
    virtual ~IUIComponent() = 0;

    virtual void Update() = 0;

    const char *GetComponentName() const;

    UIElement *GetOwnerUIElement() const;

    // 기본은 단순히 false 리턴
    // (오버라이드할시 IUIComponent 버전은 호출할필요가없음)
    virtual bool IsPointInside(float x, float y) const;
    // 빈메서드 (오버라이드할시 IUIComponent 버전은 호출할필요가없음)
    virtual void UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                       bool &captureActiveRequestFlag, bool &oCaptureReleaseRequestFlag);

    void SetDepthValue(uint32_t depth);
    uint32_t GetDepthValue() const;

  protected:
    // 기본 uiElement영역에대한 메서드
    bool IsPointInsideDefault(float x, float y) const;

  private:
    // 이 컴포넌트를 소유한 uielement, factory가 호출할것이다.
    void SetOwnerUIElement(UIElement *owner);

  private:
    const char *mName;
    UIElement *mOwnerUIElement;
    // com사이에서 유효한 깊이값
    uint32_t mDepthValue;
};

} // namespace UI
