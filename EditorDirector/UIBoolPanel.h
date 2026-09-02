#pragma once

#include <UiSystem/UIElement.h>
#include <UiSystem/UIImage.h>
#include <string>

#include "UIBoolPanel.generated.h"

namespace UI
{
class UIText;
class UIButton;
} // namespace UI

class REFLECT_CLASS(EngineClass) UIBoolPanel : public UI::UIImage
{
    GENERATED_BODY(UIBoolPanel)
  public:
    UIBoolPanel();
    virtual ~UIBoolPanel();
    virtual void OnBegin() override;
    virtual void Update(float deltaTime) override;

    void SetTagText(const std::string &tag);

    // 코드적으로 동작
    void SetCheckValue(bool value, bool bNotify = true);

    Core::MultiCallbackSystem<bool> mOnValueChanged;

  private:
    void UpdateVisitualState();

  private:
    UI::UIText *mTagText;
    UI::UIButton *mCheckButton;

    std::string mTagTextStr;
    bool mCurrentValue = false;
};
