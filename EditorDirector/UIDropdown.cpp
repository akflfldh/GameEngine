#include "UIDropdown.h"
#include <EditorDirector/EditorUIUtility.h>
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
    if (mSelectedIndex >= list.size())
        mSelectedIndex = 0;

    if (mListPanel == nullptr)
        return;

    // 목록 교체는 추가가 아니다. 이전 항목은 지연 삭제하고 새 항목만 인덱스의 기준으로 삼는다.
    for (auto *item : mItemList)
        item->Destroy();
    mItemList.clear();
    mIsListOpened = false;
    mOpenProgress = 0.0f;
    mListPanel->SetActiveFlag(false);
    mListPanel->SetHeight(0.0f);

    for (size_t i = 0; i < list.size(); ++i)
    {
        auto item = CreateItem(list[i]);
        if (item != nullptr)
            mItemList.push_back(item);
    }
    if (mItemList.empty())
        mHeaderText->SetText("");
    else
        SetSelectedIndex(mSelectedIndex, false);
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

size_t UIDropdown::GetSelectedIndex() const
{
    return mSelectedIndex;
}

bool UIDropdown::GetSelectedText(std::string &outText) const
{
    // Begin 전에도 조회할 수 있도록 자식 버튼이 아닌 저장된 항목 목록을 기준으로 삼는다.
    // 실패 시 이전 조회 결과가 남지 않도록 출력 문자열을 비운다.
    if (mSelectedIndex >= mItemTextList.size())
    {
        outText.clear();
        return false;
    }

    outText = mItemTextList[mSelectedIndex];
    return true;
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

    /*  if (mHeaderDropButton)
      {
          mHeaderDropButton->SetSize(d, d);
          mHeaderDropButton->SetPositionLocal(mTransform.GetSize().x - d, 0.0f);
      }*/
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
        //       mHeaderDropButton->SetPositionLocal(mTransform.GetSize().x - mHeaderDropButton->mTransform.GetSize().x,
        //       0.0f);
        //  mHeaderDropButton->SetHorizontalOffset(mHeaderDropButton->mTransform.GetSize().x);
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

    mHeaderHeight = h;
    if (mHeader)
    {
        //   mHeaderDropButton->SetSize(mHeaderHeight, mHeaderHeight);
        //   mHeader->SetHeight(mHeaderHeight);
        //  mHeaderText->SetHeight(mHeaderHeight);
        // mHeaderText->SetFontSize(mHeaderHeight - mHeaderHeight / 2.0f);

        // mHeaderDropButton->SetPositionLocal(mTransform.GetSize().x - mHeaderDropButton->mTransform.GetSize().x,
        // 0.0f);
        //     mHeaderDropButton->SetHorizontalOffset(h);
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

    mHeader = EditorUIUtility::CreateSectionHeader(this, "Header");

    mHeader->SetWidth(dropdownWidth);

    //     mHeader->SetSize(dropdownWidth, dropdownHeaderHeight);

    // mHeader->SetColor(0.7f, 0.18f, 0.18f);

    auto text = EditorUIUtility::CreateLabel(mHeader, "HeaderText");

    text->SetClipingMode(UI::EUITextClipingMode::eEllipsis);
    text->SetOverflowMode(UI::EUITextOverflowMode::eEllipsis);
    // EditorUIUtility의 기본 색상 유지: text->SetTextColor({1, 1, 1});
    // text->SetFontSize(20.0f);
    text->SetWidth(dropdownWidth - dropdownHeaderHeight);

    //    text->SetSize(dropdownWidth - dropdownHeaderHeight, dropdownHeaderHeight);

    // text->SetText("테스트입니다.");

    mHeaderText = text;

    auto button = EditorUIUtility::CreateSmallButton(mHeader, "HeaderButton");

    button->SetVerticalAnchor(0.0f);
    button->SetVerticalOffset((mHeader->GetHeight() - button->GetHeight()) * 0.5f);

    // button->SetSize(dropdownHeaderHeight, dropdownHeaderHeight);
    button->SetPositionLocal(dropdownWidth - button->mTransform.GetSize().x, 0.0f);
    button->mUIImageComponent->UseTexture();
    button->mUIImageComponent->SetTexture("Engine/ExpandArrowDown");
    // button->mUIImageComponent->SetColor(0.28f, 0.28f, 0.28f);
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

    button->mUIImageComponent->SetUseBorderFlag(true);
    button->mUIImageComponent->SetBorderColor(UI::UIColor::LightGray);

    mHeaderDropButton = button;
    mHeaderDropButton->SetHorizontalAnchor(1.0f);
    mHeaderDropButton->SetHorizontalOffset(-(mHeaderDropButton->GetSize().X + 0.0f));
    // 자체 Pivot이 (0, 0)이므로 기존 우측/하단 정렬은 크기를 포함한 음수 Offset으로 보존한다.
    mHeaderDropButton->mOnChangedSizeCallbackSystem.Register(
        [](UI::UIElement *element)
        {
            element->SetHorizontalOffset(-(element->GetSize().X + 0.0f));
            element->UpdatePosAnchor();
        });

    return mHeader;
}
UI::UIImage *UIDropdown::CreateListPanel()
{

    float dropdownWidth = mTransform.GetSize().x;
    float dropdownHeaderHeight = mTransform.GetSize().y;

    mListPanel = EditorUIUtility::CreatePanel(this, "ListPanel");

    mListPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalCom");
    mListPanel->SetUseScissorRect(true);
    // mListPanel->SetColor(0.12f, 0.12f, 0.12f);

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

    auto item = EditorUIUtility::CreateSmallTextButton(mListPanel, "Item");
    item->SetWidth(dropdownWidth);
    // item->SetSize(dropdownWidth, mItemHeight);
    //  item->SetHeight(mItemHeight);
    // EditorUIUtility의 기본 색상 유지: item->mTextComponent->SetColor(1, 1, 1);
    // item->mTextComponent->SetFontSize(20.0f);
    item->mTextComponent->SetText(text);
    item->mTextComponent->SetClipingMode(UI::EUITextClipingMode::eEllipsis);
    item->mTextComponent->SetOverflowMode(UI::EUITextOverflowMode::eEllipsis);
    item->SetUseScissorRect(true);
    item->mUIButtonComponent->mButtonClickCallbackSystem.Register([this, self = item](float, float)
                                                                  { SetSelectedItem(self); });
    return item;
}
