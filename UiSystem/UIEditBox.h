#pragma once

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/UIElement.h>

#include "UIEditBox.generated.h"

using OnFinishInputCallbackSystem = Core::MultiCallbackSystem<const std::string &>;
using OnStartInputCallbackSystem = Core::MultiCallbackSystem<>;
using OnEditBoxTextChangedCallbackSystem = Core::MultiCallbackSystem<const std::string &>;

namespace UI
{
class UIImageComponent;
class UITextComponent;
class UITextInputComponent;
class UIImage;
class UIMultiRectComponent;

class UISYSTEM_API REFLECT_CLASS(EngineClass) UIEditBox : public UIElement
{
    GENERATED_BODY(UIEditBox)
  public:
    UIEditBox();
    virtual ~UIEditBox();

    void SetText(const std::string &text);
    std::string GetText() const;

    void SetFontSize(float size);
    void SetTextColor(float r, float g, float b);
    void SetTextColor(const UI::UIColor &color);

    UITextComponent *GetTextComponent() const;
    void SetClipingMode(EUITextClipingMode mode);
    void SetOverflowMode(EUITextOverflowMode mode);
    void SetTextInputType(EUITextInputType mode);
    EUITextInputType GetTextInputType() const;
    void SetBackgroundColor(float r, float g, float b);

    UITextInputComponent *GetTextInputComponent() const;

    UIElement *GetCursorImageElement() const;

    void SetCursorPosByWorldPos(float worldPosX, float worldPosY);

    OnFinishInputCallbackSystem mOnFinishInputCallbackSystem;
    OnStartInputCallbackSystem mOnStartInputCallbackSystem;

    // 입력으로 변경되었을때만 호출
    OnEditBoxTextChangedCallbackSystem mOnEditBoxTextChangedCallbackSystem;

  protected:
    virtual void OnBegin() override;

  private:
    UIImageComponent *mBackgroundImageCom;
    UIImage *mCursorElement;
    UITextComponent *mTextCom;
    UITextInputComponent *mTextInputCom;
    UIMultiRectComponent *mMultiRectCom;
};
} // namespace UI
