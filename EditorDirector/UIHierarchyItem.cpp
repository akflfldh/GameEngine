#include "UIHierarchyItem.h"
#include <EditorDirector/EditorUIUtility.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIHorizontalLayoutComponent.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
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

    auto canvas = GetDestCanvas();

    // SetColor({0.3, 0.3, 0.3});

    mHeaderPanel = CreateChildUIElement<UI::UIButton>("HeaderButton");
    auto horizontialCom = mHeaderPanel->CreateUIComponent<UI::UIHorizontalLayoutComponent>("HoriCom");
    horizontialCom->SetItemPaddingTop(5.0f);

    mHeaderPanel->mHoverImageColor = UI::UIColor::DarkGray;

    mHeaderPanel->SetSize(itemWidth, mHeaderHeight);
    mHeaderPanel->SetWidth(itemWidth);

    //  mHeaderPanel->mUIImageComponent->SetColor(0.3f, 0.3f, 0.3f);
    //    mHeaderPanel->mTextComponent->SetFontSize(25.0F);

    mHeaderPanel->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float) { mOnClickedHeaderPanelCallbackSystem.ExecuteCallbacks(); });

    mExpandButton = EditorUIUtility::CreateSmallButton(mHeaderPanel, "ExpandButton");
    // mExpandButton->SetSize(25, 25);

    mExpandButton->mUIImageComponent->UseTexture();
    mExpandButton->mUIImageComponent->SetTexture("Engine/ExpandArrowRight");
    //  mExpandButton->SetPositionLocal(5, 5);

    mExpandButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float)
                                                                           { SetExpandFlag(!GetExpandFlag()); });

    mIconImage = EditorUIUtility::CreateIcon(mHeaderPanel, "IconImage");

    mIconImage->SetWidth(mIconImage->GetHeight());
    // mIconImage->SetPositionLocal(mExpandButton->GetWidth() + 5.0f, 5);
    mIconImage->SetOnlyVisible(true);
    mIconImage->RefreshStyle();

    //    mHeaderText = mHeaderPanel->CreateChildUIElement<UI::UIText>("HeaderText");
    mHeaderText = EditorUIUtility::CreateText(mHeaderPanel, "HeaderText", "", itemWidth);
    mHeaderText->SetOnlyVisible(true);
    // mHeaderText->SetHeight(30.0f);

    /* mHeaderPanel->mTextComponent->SetPaddingLeft(mIconImage->GetWidth() + mIconImage->mTransform.GetLocalPosition().x
       + 10.0f);*/

    mContentPanel = EditorUIUtility::CreatePanel(this, "ContentPanel");

    mVerticalLayoutComponent = mContentPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
    // mVerticalLayoutComponent->SetItemPaddingX(25);
    mContentPanel->SetWidth(itemWidth);
    mContentPanel->SetHeight(0.0F);
    mContentPanel->SetPositionLocal({25.0f, mHeaderHeight});
    //  mContentPanel->SetColor(0.0, 1.0, 0.0);

    mContentPanel->mOnChangedSizeCallbackSystem.Register([this](UI::UIElement *contentPanel) { UpdateHeight(); });

    mHeaderHeight = mHeaderPanel->GetHeight();
    mItemHeight = mHeaderHeight;

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

// style일변화 , hover,등 상태변화 에서 호출
void UIHierarchyItem::ApplyVisualStyle(const UI::UIControlStyle &style, UI::EUIVisualState state)
{

    SetColor(style.mBackgroundColor);
}

void UIHierarchyItem::SetHeaderText(const std::string &text)
{

    if (mHeaderText)
    {
        mHeaderText->SetText(text);
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
        return mHeaderPanel->GetHeight();
    }
}

void UIHierarchyItem::SetHeaderColor(float r, float g, float b)
{
    /*  if (mHeaderPanel)
      {
          // EditorUIUtility의 기본 색상 유지: mHeaderPanel->mUIImageComponent->SetColor(r, g, b);
      }*/
}

void UIHierarchyItem::SetWidth(float w)
{
    UI::UIImage::SetWidth(w);

    if (mHeaderPanel)
    {
        mHeaderPanel->SetWidth(w);
        mHeaderText->SetWidth(std::max(0.0f, GetWidth() - mHeaderText->mTransform.GetLocalPosition().x));
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

    if (mHeaderText)
    {
        return mHeaderText->GetText();
    }

    return "";
    // TODO: 여기에 return 문을 삽입합니다.
}

UI::UIButton *UIHierarchyItem::GetHeaderPanel() const
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
