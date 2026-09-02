#include "UIDropdown.h"
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UIVerticalLayoutComponent.h>

UIDropdown::UIDropdown()
{
    // VerticalLayoutCom을통해 List창이 active가 변할때마다 자동으로 height를 조절이 가능하다.
    CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalCom");
}
UIDropdown ::~UIDropdown() {}

void UIDropdown::SetItemList(const std::vector<std::string> &list)
{

    mItemTextList = list;

    if (mListPanel == nullptr)
        return;

    // 일단 주석처리
    //  ClearItemList();

    for (size_t i = 0; i < list.size(); ++i)
    {
        auto item = CreateItem(list[i]);
        if (item != nullptr)
            mItemList.push_back(item);
    }
}

void UIDropdown::SetItemHeight(float h)
{

    mItemHeight = h;

    if (mIsBegun)
    {
        for (auto item : mItemList)
        {
            item->SetHeight(mItemHeight);
        }
    }
}

void UIDropdown::SetSelectedIndex(size_t index, bool bNotify)
{

    mSelectedIndex = index;

    if (mItemList.size() <= index)
        return;

    // 헤더를 업데이트
    mHeaderText->SetText(mItemList[index]->mTextComponent->GetText());

    // 리스트창을 닫는다.
    Close();

    // Callback 호출
    if (bNotify)
        mOnSelectedItemChangedCallbackSystem.ExecuteCallbacks(index);
}

void UIDropdown::SetSelectedItem(UI::UITextButton *item)
{

    auto it = std::find(mItemList.begin(), mItemList.end(), item);

    if (it != mItemList.end())
    {
        SetSelectedIndex(it - mItemList.begin());
    }
}

void UIDropdown::SetHeaderButtonSize(float d)
{

    if (mHeaderDropButton)
    {
        mHeaderDropButton->SetSize(d, d);
        mHeaderDropButton->SetPositionLocal(mTransform.GetSize().x - d, 0.0f);
    }
}

void UIDropdown::SetSize(float w, float h)
{
    UIElement::SetSize(w, h);
}

void UIDropdown::SetWidth(float w)
{
    UIElement::SetWidth(w);
    if (mHeader)
    {
        mHeader->SetWidth(w);

        mHeaderText->SetWidth(w - mHeaderDropButton->mTransform.GetSize().x);
        // mHeaderDropButton->SetPositionLocal(mTransform.GetSize().x - mHeaderDropButton->mTransform.GetSize().x,
        // 0.0f);
        //  mHeaderDropButton->SetHorizontalPivotOffset(mHeaderDropButton->mTransform.GetSize().x);
    }

    if (mListPanel)
    {
        mListPanel->SetWidth(w);
    }

    for (auto item : mItemList)
    {
        if (item)
        {
            item->SetWidth(w);
        }
    }
}

void UIDropdown::SetHeaderHeight(float h)
{

    if (mHeader)
    {
        mHeaderDropButton->SetSize(h, h);
        mHeader->SetHeight(h);
        mHeaderText->SetHeight(h);

        // mHeaderDropButton->SetPositionLocal(mTransform.GetSize().x - mHeaderDropButton->mTransform.GetSize().x,
        // 0.0f);
        //     mHeaderDropButton->SetHorizontalPivotOffset(h);
    }
}

void UIDropdown ::ApplyOpenProgress()
{
    if (mListPanel == nullptr)
        return;

    float fullHeight = mItemList.size() * mItemHeight;

    float currHeight = fullHeight * mOpenProgress;

    mListPanel->SetHeight(currHeight);

    if (mOpenProgress == 0.0f)
    {
        mListPanel->SetActiveFlag(false);
    }
}

void UIDropdown::OnBegin()
{
    SetUseScissorRect(true);
    CreateHeaderPanel();

    CreateListPanel();

    if (mItemList.empty() && mItemTextList.empty() == false)
    {
        for (size_t i = 0; i < mItemTextList.size(); ++i)
        {
            auto item = CreateItem(mItemTextList[i]);
            if (item != nullptr)
                mItemList.push_back(item);
        }

        SetSelectedIndex(mSelectedIndex, false);
    }
}

void UIDropdown::Update(float DeltaTime)
{
    UI::UIElement::Update(DeltaTime);
    float targetOpen = mIsListOpened ? 1.0f : 0.0f;

    if (targetOpen > mOpenProgress)
    {
        // open 진행
        mOpenProgress += DeltaTime * mOpenSpeed;

        if (mOpenProgress >= 1.0F)
        {
            mOpenProgress = 1.0f;
        }
        ApplyOpenProgress();
    }
    else if (targetOpen < mOpenProgress)
    {
        // close 진행
        mOpenProgress -= DeltaTime * mOpenSpeed;

        if (mOpenProgress <= 0.0f)
            mOpenProgress = 0.0f;

        ApplyOpenProgress();
    }
}

// 리스트 판넬을 펼친다.
void UIDropdown::Open()
{
    if (mListPanel)
    {
        mListPanel->SetActiveFlag(true);
        mIsListOpened = true;

        // verticalLayout의 자식들의 사이즈 변화로 초기화다음프레임에 높이가 자동설정됨으로
        // 그냥 항상 open 시작에 높이값을 조정하자.
        if (mOpenProgress == 0.0f)
            mListPanel->SetHeight(0.0f);
    }
}
// 리스트 판넬을 접는다.
void UIDropdown::Close()
{

    if (mListPanel)
    {
        //   mListPanel->SetActiveFlag(false);
        mIsListOpened = false;
    }
}

UI::UIImage *UIDropdown::CreateHeaderPanel()
{

    float dropdownWidth = mTransform.GetSize().x;
    float dropdownHeaderHeight = mTransform.GetSize().y;

    mHeader = CreateChildUIElement<UI::UIImage>("Header");
    mHeader->SetSize(dropdownWidth, dropdownHeaderHeight);
    mHeader->SetColor(0.7f, 0.18f, 0.18f);

    auto text = mHeader->CreateChildUIElement<UI::UIText>("HeaderText");
    text->SetClipingMode(UI::EUITextClipingMode::eEllipsis);
    text->SetOverflowMode(UI::EUITextOverflowMode::eEllipsis);
    text->SetTextColor({1, 1, 1});
    text->SetFontSize(20.0f);
    text->SetSize(dropdownWidth - dropdownHeaderHeight, dropdownHeaderHeight);
    text->SetText("테스트입니다.");

    mHeaderText = text;

    auto button = mHeader->CreateChildUIElement<UI::UIButton>("HeaderButton");
    button->SetSize(dropdownHeaderHeight, dropdownHeaderHeight);
    button->SetPositionLocal(dropdownWidth - button->mTransform.GetSize().x, 0.0f);
    button->mUIImageComponent->SetColor(0.28f, 0.28f, 0.28f);
    button->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            if (IsListOpened())
            {
                Close();
            }
            else
            {
                Open();
            }
        });

    mHeaderDropButton = button;
    mHeaderDropButton->SetHorizontalPivotSide(UI::EUIPosPivotHorizontal::eRight);
    mHeaderDropButton->SetHorizontalPivotOffset(0.0f);

    return mHeader;
}
UI::UIImage *UIDropdown::CreateListPanel()
{

    float dropdownWidth = mTransform.GetSize().x;
    float dropdownHeaderHeight = mTransform.GetSize().y;

    mListPanel = CreateChildUIElement<UI::UIImage>("ListPanel");
    mListPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalCom");
    mListPanel->SetUseScissorRect(true);
    mListPanel->SetColor(0.12f, 0.12f, 0.12f);

    mListPanel->SetSize(dropdownWidth, 0.0f);

    mListPanel->SetPositionLocal(0.0f, dropdownHeaderHeight);
    mListPanel->SetActiveFlag(false);

    return mListPanel;
}

bool UIDropdown::IsListOpened() const
{

    return mIsListOpened;
}

UI::UITextButton *UIDropdown::CreateItem(const std::string &text)
{
    float dropdownWidth = mTransform.GetSize().x;

    if (mListPanel == nullptr)
        return nullptr;

    auto item = mListPanel->CreateChildUIElement<UI::UITextButton>("Item");
    item->SetSize(dropdownWidth, mItemHeight);
    item->SetHeight(mItemHeight);
    item->mTextComponent->SetColor(1, 1, 1);
    item->mTextComponent->SetFontSize(20.0f);
    item->mTextComponent->SetText(text);
    item->mTextComponent->SetClipingMode(UI::EUITextClipingMode::eEllipsis);
    item->mTextComponent->SetOverflowMode(UI::EUITextOverflowMode::eEllipsis);
    item->SetUseScissorRect(true);
    item->mUIButtonComponent->mButtonClickCallbackSystem.Register([this, self = item](float, float)
                                                                  { SetSelectedItem(self); });
    return item;
}
