#pragma once

#include <UiSystem/UIImage.h>

#include "UIFoldoutPanel.generated.h"

namespace UI
{
class UIVerticalLayoutComponent;
class UIText;
class UIButton;
} // namespace UI

class REFLECT_CLASS(EngineClass) UIFoldoutPanel : public UI::UIImage
{
    GENERATED_BODY(UIFoldoutPanel)
  public:
    UIFoldoutPanel();
    virtual ~UIFoldoutPanel();

    virtual void OnBegin() override;
    virtual void Update(float deltaTime) override;

    void SetHeaderText(const std::string &text);

    void SetExpanded(bool bExpand);

    void AddItem(UI::UIElement *item);

  private:
    void UpdatePanelSize();

    void UpdateButtonVisitual();
    virtual void OnTransformChanged(UI::ETransformChangeType type) override;

  protected:
    UI::UIImage *mHeaderPanel;
    UI::UIText *mHeaderTagText;
    UI::UIButton *mHeaderButton;

    UI::UIImage *mContentPanel;
    UI::UIVerticalLayoutComponent *mVerticalLayoutCom;

    bool mIsExpanded = false;
};
