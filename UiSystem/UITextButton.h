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

    void SetText(const std::string &text);
    UITextComponent *mTextComponent;

  protected:
    // style일변화 , hover,등 상태변화 에서 호출
    virtual void ApplyVisualStyle(const UIControlStyle &style, EUIVisualState visualState) override;
    virtual void OnUpdatedAutoSize(float scale) override;

  private:
};

} // namespace UI
