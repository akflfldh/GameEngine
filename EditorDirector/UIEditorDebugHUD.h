#pragma once

#include <UiSystem/UIElement.h>
#include <vector>

#include "UIEditorDebugHUD.generated.h"
namespace UI
{
class UITextComponent;
class UIImageComponent;
class UIVerticalLayoutComponent;
} // namespace UI

class REFLECT_CLASS(EngineClass) UIEditorDebugHUD : public UI::UIElement
{
    GENERATED_BODY(UIEditorDebugHUD)
  public:
    UIEditorDebugHUD();
    virtual ~UIEditorDebugHUD();

    virtual void Update(float deltaTime) override;

    void AddText(const std::string &str, int lineIndex = 0);

  protected:
    virtual void OnBegin() override;

  private:
    UI::UIImageComponent *mBackgroundImageComponent;
    UI::UIVerticalLayoutComponent *mVerticalLayoutComponent;

    UI::UITextComponent *mTextComponent;
    UI::UITextComponent *mAddedTextComponent;

    float mUpdateTime = 0.0f;

    std::vector<std::string> mAddedTextList;
};
