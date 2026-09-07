#pragma once

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/UIElement.h>
#include <UiSystem/UISystemDllMacro.h>

#include "UIButton.generated.h"

using ButtonHoverCallbackSystem = Core::MultiCallbackSystem<float, float>;
using ButtonReleaseHoverCallbackSystem = Core::MultiCallbackSystem<>;

namespace UI
{

class UIButtonComponent;
class UIImageComponent;

class UISYSTEM_API REFLECT_CLASS(EngineClass) UIButton : public UIElement
{
    GENERATED_BODY(UIButton)

  public:
    UIButton();
    virtual ~UIButton();
    virtual void Update(float deltaTime) override;
    virtual void OnBegin() override;

    virtual void OnHover(int x, int y) override;
    virtual void OnReleaseHover() override;

    UIButtonComponent *mUIButtonComponent;
    UIImageComponent *mUIImageComponent;

    ButtonHoverCallbackSystem mHoverCallbackSystem;
    ButtonReleaseHoverCallbackSystem mReleaseHoverCallbackSystem;
    // down callbacksystem 은 buttonCom에 존재

    UI::UIColor mHoverImageColor;
    UI::UIColor mReleaseHoverImageColor;

    void SetUseHoverImageColor(bool flag);

  protected:
    // style 변화에서 호출
    virtual void ApplyLayoutStyle(const UIControlStyle &style) override;

    // style일변화 , hover,등 상태변화 에서 호출
    virtual void ApplyVisualStyle(const UIControlStyle &style, EUIVisualState visualState) override;

    virtual EUIVisualState ResolveVisualState() const;

  private:
    bool mUseHoverImageColor;
};

} // namespace UI