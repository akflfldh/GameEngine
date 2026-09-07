#pragma once

#include <UiSystem/UIButton.h>

#include "UITextButton.generated.h"

namespace UI
{

class UITextComponent;

class UISYSTEM_API REFLECT_CLASS(EngineClass) UITextButton : public UIButton
{
    GENERATED_BODY(UITextButton)
  public:
    UITextButton();
    virtual ~UITextButton();

    virtual void Update(float deltaTime) override;
    virtual void OnBegin() override;

    UITextComponent *mTextComponent;

  protected:
    // style 변화에서 호출
    virtual void ApplyLayoutStyle(const UIControlStyle &style) override;

    // style일변화 , hover,등 상태변화 에서 호출
    virtual void ApplyVisualStyle(const UIControlStyle &style, EUIVisualState visualState) override;





  private:
};

} // namespace UI