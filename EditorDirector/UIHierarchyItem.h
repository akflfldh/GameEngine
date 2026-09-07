#pragma once

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/UIImage.h>

#include "UIHierarchyItem.generated.h"

using OnClickedHeaderPanelCallbackSystem = Core::MultiCallbackSystem<>;

namespace UI
{
class UIVerticalLayoutComponent;
class UITextButton;
class UIButton;
class UIText;
class UIImage;
} // namespace UI

class REFLECT_CLASS(EngineClass) UIHierarchyItem : public UI::UIImage
{
    GENERATED_BODY(UIHierarchyItem)
  public:
    UIHierarchyItem();
    virtual ~UIHierarchyItem();
    virtual void OnBegin() override;

    void SetHeaderHeight(float h);
    void SetHeaderText(const std::string &text);
    std::string GetHeaderText() const;
    UI::UITextButton *GetHeaderPanel() const;

    void AddItem(UI::UIElement *element);
    void RemoveItem(UI::UIElement *element);
    void RemoveItemAll();

    const std::vector<UIElement *> &GetItemList() const;

    OnClickedHeaderPanelCallbackSystem mOnClickedHeaderPanelCallbackSystem;

    void SetHeaderFontSize(float size);
    float GetHeaderLineHeight() const;

    void SetHeaderColor(float r, float g, float b);

    virtual void SetWidth(float w) override;

    void SetExpandFlag(bool flag);
    bool GetExpandFlag() const;

  protected:
    void UpdateHeight();
    // style 변화에서 호출
    virtual void ApplyLayoutStyle(const UI::UIControlStyle &style) override;

    // style일변화 , hover,등 상태변화 에서 호출
    virtual void ApplyVisualStyle(const UI::UIControlStyle &style, UI::EUIVisualState state) override;

    // UI::UIButton *mHeaderPanel;
    // UI::UIText *mHeaderText;
    UI::UITextButton *mHeaderPanel; // 해당오브젝트에대한 판넬
    UI::UIButton *mExpandButton;
    UI::UIImage *mIconImage = nullptr;
    UI::UIImage *mContentPanel; // 자식들에대한 리스트 판넬

    UI::UIVerticalLayoutComponent *mVerticalLayoutComponent;
    float mHeaderHeight;

    bool mIsExpanded;

    float mItemHeight;
};
