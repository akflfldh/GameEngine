#pragma once

#include <CoreBase/CallbackSystem.h>
#include <ReflectSystem/ReflectionMacro.h>
#include <UiSystem/UIElement.h>
#include <vector>
namespace UI
{
class UIImageComponent;
class UIMovableComponent;
} // namespace UI

#include "UIScrollBox.generated.h"

enum class EUIScrollLayout
{
    eVertical,
    eGrid
};

using OnBackgroundClickedCallbackSystem = Core::MultiCallbackSystem<>;

class REFLECT_CLASS(EngineClass) UIScrollBox : public UI::UIElement
{
    GENERATED_BODY(UIScrollBox)
  public:
    UIScrollBox();
    virtual ~UIScrollBox();

    virtual void OnBegin() override;

    void SetBackgrounColor(float r, float g, float b);

    void AddItem(const std::string &str);
    void AddItem(UI::UIElement *itemElement);
    void AddItemList(const std::vector<UI::UIElement *> &itemElementList);
    void RemoveItem(const std::string &str);
    void RemoveItem(UI::UIElement *item);

    UI::UIElement *RemoveItem(int index, bool bDestory = true);
    std::vector<UI::UIElement *> RemoveItemAll(bool bDestory = true);

    void SetItemTextSize(float size);
    void SetItemHeight(float h);

    virtual void OnTransformChanged(UI::ETransformChangeType type) override;

    void SetLayout(EUIScrollLayout layout);

    const std::vector<UIElement *> &GetItemList() const;

    void ForceUpdateLayout();

    void SetItemSize(float w, float h);

    OnBackgroundClickedCallbackSystem mOnBackgroudClickedCallbackSystem;

  private:
    float GetNextItemY() const;
    void UpdateLayout();

    void UpdateContentLayout();

  private:
    UI::UIImageComponent *mImageComponent;
    UI::UIMovableComponent *mMovableComponent;
    UI::UIElement *mContentPanel;
    UI::UIElement *mScrollControlPanel;
    float mItemFontSize;
    float mItemHeight;
    float mItemStartYOffsetLocal;
    // std::vector<UI::UIElement *> mItemVec;

    EUIScrollLayout mLayout;
};
