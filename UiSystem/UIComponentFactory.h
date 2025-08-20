#pragma once

#include "UiSystem/UISystemDllMacro.h"
#include <CoreMath/CoreMath.h>
namespace UI
{
class IUIComponent;
class UIElement;

class UISYSTEM_API UIComponentFactory
{
  public:
    static UIComponentFactory *GetInstance();
    UIComponentFactory();
    ~UIComponentFactory();

    UI::IUIComponent *Create(UIElement *uiElement, const char *componentStaticName, const char *componentInstanceName);
    void Release(IUIComponent *component);

    bool IsBaseClass(const char *baseClassName, const char *childClassName) const;

  private:
};

} // namespace UI