#pragma once
#include <UiSystem/UISystemDllMacro.h>

namespace UI
{
class UIElement;

class UISYSTEM_API UIUtility
{
  public:
    ~UIUtility() = default;

    // startPosX + element.size().x + margin
    static float ShiftPosX(float startPosX, UIElement *element, float margin = 0.0f);
    static float ShiftPosY(float startPosY, UIElement *element, float margin = 0.0f);

  private:
    UIUtility() = default;
};

} // namespace UI