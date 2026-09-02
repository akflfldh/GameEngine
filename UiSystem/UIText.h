#pragma once
#include <UiSystem/UIElement.h>

#include "UIText.generated.h"

namespace UI
{
class UITextComponent;
class UISYSTEM_API REFLECT_CLASS(EngineClass) UIText : public UIElement
{
    GENERATED_BODY(UIText)

  public:
    UIText();
    virtual ~UIText();
    virtual void OnBegin() override;

    void SetText(const std::string &text);
    void SetFontSize(float size);
    EUITextClipingMode GetClipingMode() const;
    void SetClipingMode(EUITextClipingMode mode);

    void SetOverflowMode(EUITextOverflowMode mode);
    EUITextOverflowMode GetOverflowMode() const;

    UITextComponent *GetTextComponent() const;

    void SetTextColor(const UI::UIColor &color);

  protected:
    UITextComponent *mTextComponent;

  private:
};

} // namespace UI