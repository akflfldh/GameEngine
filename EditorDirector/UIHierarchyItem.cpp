#include "UIHierarchyItem.h"
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UIVerticalLayoutComponent.h>
UIHierarchyItem::UIHierarchyItem()
    : mHeaderHeight(30.0f), mExpandButton(nullptr), mHeaderPanel(nullptr), mContentPanel(nullptr), mIsExpanded(false)
{
    SetStyleRole(UI::EUIStyleRole::eListItem);
}

UIHierarchyItem ::~UIHierarchyItem() {}

void UIHierarchyItem::OnBegin()
{

    UI::UIImage::OnBegin();
    float itemWidth = mTransform.GetSize().x;

    // SetColor({0.3, 0.3, 0.3});

    mHeaderPanel = CreateChildUIElement<UI::UITextButton>("HeaderPanel");
    mHeaderPanel->SetStyleRole(UI::EUIStyleRole::eListItem);
    // mHeaderPanel->SetSize(itemWidth, mHeaderHeight);
    mHeaderPanel->SetWidth(itemWidth);

    mHeaderHeight = mHeaderPanel->GetHeight();

    //  mHeaderPanel->mUIImageComponent->SetColor(0.3f, 0.3f, 0.3f);
    //    mHeaderPanel->mTextComponent->SetFontSize(25.0F);

    mHeaderPanel->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float) { mOnClickedHeaderPanelCallbackSystem.ExecuteCallbacks(); });

    mExpandButton = mHeaderPanel->CreateChildUIElement<UI::UIButton>("ExpandButton");
    // mExpandButton->SetSize(25, 25);

    mExpandButton->mUIImageComponent->UseTexture();
    mExpandButton->mUIImageComponent->SetTexture("Engine/ExpandArrowRight");
    mExpandButton->SetPositionLocal(5, 5);

    mExpandButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float)
                                                                           { SetExpandFlag(!GetExpandFlag()); });

    mIconImage = mHeaderPanel->CreateChildUIElement<UI::UIImage>("IconImage");
    mIconImage->SetStyleRole(UI::EUIStyleRole::eIcon);
    mIconImage->SetWidth(mIconImage->GetHeight());
    mIconImage->SetPositionLocal(mExpandButton->GetWidth() + 5.0f, 5);

    mIconImage->RefreshStyle();

    mHeaderPanel->mTextComponent->SetPaddingLeft(mIconImage->GetWidth() + mIconImage->mTransform.GetLocalPosition().x +
                                                 10.0f);

    mContentPanel = CreateChildUIElement<UI::UIImage>("ContentPanel");
    mContentPanel->SetStyleRole(UI::EUIStyleRole::ePanel);
    mVerticalLayoutComponent = mContentPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
    // mVerticalLayoutComponent->SetItemPaddingX(25);
    mContentPanel->SetWidth(itemWidth);
    mContentPanel->SetHeight(0.0F);
    mContentPanel->SetPositionLocal({25.0f, mHeaderHeight});
    //  mContentPanel->SetColor(0.0, 1.0, 0.0);

    mContentPanel->mOnChangedSizeCallbackSystem.Register([this](UI::UIElement *contentPanel) { UpdateHeight(); });

    // SetHeight(mHeaderHeight);

    // UpdateLayout
}

void UIHierarchyItem::SetHeaderHeight(float h)
{

    /*mHeaderHeight = h;

    if (mHeaderPanel)
    {
        mHeaderPanel->SetHeight(mHeaderHeight);
        mContentPanel->SetPositionLocal({0, mHeaderHeight});
        UpdateHeight();
    }*/
    // UpdateLayout();
}
void UIHierarchyItem::UpdateHeight()
{

    float h = mHeaderHeight;
    if (mContentPanel)
    {
        h += mContentPanel->mTransform.GetSize().y;
    }

    SetHeight(h);
}

void UIHierarchyItem::ApplyLayoutStyle(const UI::UIControlStyle &style)
{

    SetHeight(style.mHeight);
    mItemHeight = style.mHeight;
}

// style일변화 , hover,등 상태변화 에서 호출
void UIHierarchyItem::ApplyVisualStyle(const UI::UIControlStyle &style, UI::EUIVisualState state)
{

    SetColor(style.mBackgroundColor);
}

void UIHierarchyItem::SetHeaderText(const std::string &text)
{

    if (mHeaderPanel)
    {
        mHeaderPanel->mTextComponent->SetText(text);
    }
}

void UIHierarchyItem::AddItem(UI::UIElement *element)
{

    if ((element == nullptr) || (mContentPanel == nullptr))
        return;
    float itemWidth = mTransform.GetSize().x;

    element->SetParent(mContentPanel);

    // element->SetPositionLocal(itemWidth, 0.0f);
    //  mVerticalLayoutComponent->CalculateLayout();
    //     element->SetPositionLocal(itemWidt, element->mTransform.GetLocalPosition().y);
}

void UIHierarchyItem::SetHeaderFontSize(float size)
{
    /* if (mHeaderPanel)
     {
         mHeaderPanel->mTextComponent->SetFontSize(size);
     }*/
}

float UIHierarchyItem::GetHeaderLineHeight() const
{
    if (mHeaderPanel)
    {
        return mHeaderPanel->mTextComponent->GetLineHeight();
    }
}

void UIHierarchyItem::SetHeaderColor(float r, float g, float b)
{
    /*  if (mHeaderPanel)
      {
          mHeaderPanel->mUIImageComponent->SetColor(r, g, b);
      }*/
}

void UIHierarchyItem::SetWidth(float w)
{
    UI::UIImage::SetWidth(w);

    if (mHeaderPanel)
    {
        mHeaderPanel->SetWidth(w);
    }

    if (mContentPanel)
    {
        const float contentWidth = w - 25.0f;

        mContentPanel->SetWidth(contentWidth);
        //   mContentPanel->SetWidth(w);

        for (auto item : mContentPanel->GetChildVector())
        {
            item->SetWidth(contentWidth);
        }
    }
}

void UIHierarchyItem::SetExpandFlag(bool flag)
{

    mIsExpanded = flag;
    if (mIsExpanded)
    {
        mExpandButton->mUIImageComponent->SetTexture("Engine/ExpandArrowDown");
        mContentPanel->SetActiveFlag(true);
        float h = mItemHeight + mContentPanel->mTransform.GetSize().y;
        SetHeight(h);
    }
    else
    {
        mExpandButton->mUIImageComponent->SetTexture("Engine/ExpandArrowRight");
        mContentPanel->SetActiveFlag(false);
        SetHeight(mItemHeight);
    }
}

bool UIHierarchyItem::GetExpandFlag() const
{
    return mIsExpanded;
}

std::string UIHierarchyItem::GetHeaderText() const
{

    if (mHeaderPanel)
    {
        return mHeaderPanel->mTextComponent->GetText();
    }

    return "";
    // TODO: 여기에 return 문을 삽입합니다.
}

UI::UITextButton *UIHierarchyItem::GetHeaderPanel() const
{
    return mHeaderPanel;
}

void UIHierarchyItem::RemoveItem(UI::UIElement *element)
{

    bool bChild = false;
    if (mContentPanel)
    {
        for (auto child : mContentPanel->GetChildVector())
        {
            if (child == element)
            {
                bChild = true;
            }
        }
    }

    if (bChild)
    {
        element->SetParent(nullptr);
    }
}

void UIHierarchyItem::RemoveItemAll()
{

    if (mContentPanel)
    {
        for (auto child : mContentPanel->GetChildVector())
        {
            child->SetParent(nullptr);
        }
    }
}

const std::vector<UI::UIElement *> &UIHierarchyItem::GetItemList() const
{

    return mContentPanel->GetChildVector();
}
