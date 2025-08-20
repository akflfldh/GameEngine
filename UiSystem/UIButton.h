#pragma once

#include <UiSystem/Generated/UIButton.generated.h>
#include <UiSystem/UIElement.h>
#include <UiSystem/UISystemDllMacro.h>

namespace UI
{

class UIButtonComponent;
class UIImageComponent;

class UISYSTEM_API REFLECT_CLASS UIButton : public UIElement
{
    GENERATED_BODY(UIButton)

  public:
    UIButton();
    virtual ~UIButton();

    virtual void OnClick();

  protected:
  private:
    UIButtonComponent *mUIButtonComponent;
    UIImageComponent *mUIImageComponent;
};

} // namespace UI