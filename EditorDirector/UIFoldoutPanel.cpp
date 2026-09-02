#include "UIFoldoutPanel.h"
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UIVerticalLayoutComponent.h>

UIFoldoutPanel::UIFoldoutPanel()
    : mContentPanel(nullptr), mHeaderPanel(nullptr), mHeaderButton(nullptr), mHeaderTagText(nullptr)
{
}

UIFoldoutPanel::~UIFoldoutPanel() {}

void UIFoldoutPanel::OnBegin()
{

    UI::UIImage::OnBegin();
    SetHeight(100.0f);

    mHeaderPanel = CreateChildUIElement<UI::UIImage>("HeaderPanel");
    mHeaderPanel->SetWidth(mTransform.GetSize().r);
    mHeaderPanel->SetHeight(50.0f);
    mHeaderPanel->SetColor({1.0f, 0.4f, 0.4f});

    /*mOnChangedSizeCallbackSystem.Register([this](UI::UIElement *parent)
                                          { mHeaderPanel->SetWidth(mTransform.GetSize().r); });*/
    // WIDTH 는 OnTransformChanged에서 동일하게

    mHeaderTagText = mHeaderPanel->CreateChildUIElement<UI::UIText>("HeaderText");
    mHeaderTagText->SetHeight(30.0f);
    mHeaderTagText->SetPositionLocal(10, 5);
    mHeaderTagText->SetTextColor({0, 0, 0});

    mHeaderButton = mHeaderPanel->CreateChildUIElement<UI::UIButton>("HeaderButton");
    mHeaderButton->SetSize(30, 30);
    mHeaderButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float)
                                                                           { SetExpanded(!mIsExpanded); });
    mHeaderButton->mUIImageComponent->UseTexture();

    mContentPanel = CreateChildUIElement<UI::UIImage>("ContentPanel");
    mVerticalLayoutCom = mContentPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
    mContentPanel->SetPositionLocal(0, 50.0f);
    mContentPanel->SetColor(0.4f, 0.4f, 0.4f);

    SetExpanded(false);
    UpdatePanelSize();
}

void UIFoldoutPanel::Update(float deltaTime)
{

    UI::UIImage::Update(deltaTime);
}

void UIFoldoutPanel::SetHeaderText(const std::string &text)
{
    if (mHeaderTagText)
    {
        mHeaderTagText->SetText(text);
        mHeaderButton->SetPositionLocal(
            {text.size() * 10.0f + mHeaderTagText->mTransform.GetLocalPosition().x + 30.0f, 5});
    }
}

void UIFoldoutPanel::SetExpanded(bool bExpand)
{
    mIsExpanded = bExpand;
    UpdateButtonVisitual();

    if (bExpand)
    {
        if (mContentPanel)
        {
            mContentPanel->SetActiveFlag(true);
        }
    }
    else
    {
        if (mContentPanel)
        {
            mContentPanel->SetActiveFlag(false);
        }
    }

    UpdatePanelSize();
}

void UIFoldoutPanel::AddItem(UI::UIElement *item)
{

    if (mContentPanel && item)
    {
        item->SetParent(mContentPanel);
        mVerticalLayoutCom->CalculateLayout();

        if (mContentPanel->GetActiveFlag())
            UpdatePanelSize();
    }
}

void UIFoldoutPanel::UpdatePanelSize()
{

    // 높이  =  헤더높이 + 콘텐츠 높이
    float height = mHeaderPanel->mTransform.GetSize().g + mContentPanel->mTransform.GetSize().g;
    SetHeight(height);

    // UIFoldoutPanel의 부모가 만약 필요하다면 알아서 callback을 등록해서 처리할것이다. .그러니 여기서 더이상할것이없다.
}

void UIFoldoutPanel::UpdateButtonVisitual()
{

    if (mIsExpanded)
    {

        //  mHeaderButton->mUIImageComponent->SetTexture("/Engine/TriArrowDown");
    }
    else
    {
        //   mHeaderButton->mUIImageComponent->SetTexture("/Engine/TriArrowRight");
    }
}

void UIFoldoutPanel::OnTransformChanged(UI::ETransformChangeType type)
{

    UI::UIImage::OnTransformChanged(type);

    if (type == UI::ETransformChangeType::eSize)
    {

        if (mHeaderPanel && (mHeaderPanel->mTransform.GetSize().r != mTransform.GetSize().r))
            mHeaderPanel->SetWidth(mTransform.GetSize().r);

        if (mContentPanel && (mContentPanel->mTransform.GetSize().r != mTransform.GetSize().r))
        {
            mContentPanel->SetWidth(mTransform.GetSize().r);

            for (auto item : mContentPanel->GetChildVector())
            {
                item->SetWidth(mTransform.GetSize().r);
            }
        }
    }
}
