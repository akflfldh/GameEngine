#pragma once
#include <UiSystem/UISystemDllMacro.h>
#include <string>

namespace UI
{
class UIElement;
class UIText;
class UITextButton;

class UISYSTEM_API UIUtility
{
  public:
    ~UIUtility() = default;

    // startPosX + element.size().x + margin
    static float ShiftPosX(float startPosX, UIElement *element, float margin = 0.0f);
    static float ShiftPosY(float startPosY, UIElement *element, float margin = 0.0f);

    static UIText *CreateText(UIElement *parent, const char *instanceName, const std::string &text, float width);

    static UITextButton *CreateTextButton(UIElement *parent, const char *instanceName, const std::string &text,
                                          float width);

    // width 가 0.0f이 아니면 설정 , 0.0f이면 부모 너비
    static UIElement *CreateHorizontalRow(UIElement *parent, const std::string &instanceName, float width = 0.0f,
                                          bool bBackground = false);

  private:
    UIUtility() = default;
};

} // namespace UI