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

  private:
};

} // namespace UI