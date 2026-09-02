#include "UISearchSelectBox.h"

#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIMouseWheelComponent.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UITextInputComponent.h>
#include <UiSystem/UIVerticalLayoutComponent.h>

UISearchSelectBox::UISearchSelectBox() {}
UISearchSelectBox::~UISearchSelectBox() {}
void UISearchSelectBox::Update(float deltaTime)
{
    UI::UIImage::Update(deltaTime);

    if (mPendingClearVisibleItems)
    {

        ClearVisibleitemsImmediately();
        mPendingClearVisibleItems = false;
    }
}

void UISearchSelectBox::SetItems(const std::vector<SearchSelectItem> &items)
{
    ClearItems();
    mItems = items;
}

void UISearchSelectBox::ClearItems()
{
    mItems.clear();

    for (auto itemButton : mItemButtons)
    {
        itemButton->SetActiveFlag(false);
        itemButton->SetParent(nullptr);
        itemButton->mUIButtonComponent->mButtonClickCallbackSystem.UnRegisterAll();
        mItemButtonPool.push_back(itemButton);
    }
    mItemButtons.clear();

    mFilteredItems.clear();
    mPendingClearVisibleItems = false;
    HideListPanel();
}

void UISearchSelectBox::SetListPanelHeight(float h)
{

    if (mListViewportPanel)
    {
        mListViewportPanel->SetHeight(h);
    }

    SetHeight(30.0f + h);
}

void UISearchSelectBox::SetWidth(float w)
{

    UI::UIImage::SetWidth(w);

    if (mEditBox)
    {
        mEditBox->SetWidth(w);
    }

    if (mListViewportPanel)
    {
        mListViewportPanel->SetWidth(w);
    }

    if (mListContentPanel)
    {
        mListContentPanel->SetWidth(w);
        for (auto itemButton : mItemButtons)
        {
            itemButton->SetWidth(w);
        }
    }
}

void UISearchSelectBox::ClearSelectedText()
{

    SetSelectedText("");
}
void UISearchSelectBox::SetSelectedText(const std::string &text)
{

    if (mEditBox)
    {
        mEditBox->SetText(text);
    }

    ClearVisibleitemsImmediately();
}

void UISearchSelectBox::OnBegin()
{

    UI::UIImage::OnBegin();

    Initialize();

    CreateEditBox();

    CreateListPanel();
}

void UISearchSelectBox::Initialize()
{

    // editbox size
    SetHeight(30.0f);
}

void UISearchSelectBox::CreateEditBox()
{

    float width = mTransform.GetSize().x;

    mEditBox = CreateChildUIElement<UI::UIEditBox>("EditBox");
    mEditBox->SetTextColor(0, 0, 0);
    mEditBox->SetBackgroundColor(0.2f, 0.2F, 0.2F);
    mEditBox->SetSize(width, 30.0f);
    mEditBox->SetFontSize(20.0f);
    mEditBox->SetKeyboardCaptureScope(this);
    //  mEditBox->SetTextInputType(UI::EUITextInputType::eString);

    mEditBox->mOnEditBoxTextChangedCallbackSystem.Register([this](const std::string &str)
                                                           { OnSearchTextChanged(str); });

    mEditBox->mOnFinishInputCallbackSystem.Register([this](const std::string &str) { OnFinishInput(str); });

    // 나중에 고려해볼것
    //  EditBox 엔터입력에대한 콜백도  받는다.
    //  해당텍스트에 일치하는 목록이있는지 찾아서 선택, 일치하는 목록이없을떄는 그냥  EditBox를 빈텍스트로 설정해버린다.
}

void UISearchSelectBox::CreateListPanel()
{

    float width = mTransform.GetSize().x;

    mListViewportPanel = CreateChildUIElement<UI::UIImage>("ListPanel");
    mListViewportPanel->SetWidth(width);
    mListViewportPanel->SetHeight(100.0f);
    mListViewportPanel->SetUseScissorRect(true);
    mListViewportPanel->SetPositionLocal(0, mEditBox->mTransform.GetSize().y);

    mListViewportPanel->SetActiveFlag(false);
    UI::UIMouseWheelComponent *wheelCom =
        mListViewportPanel->CreateUIComponent<UI::UIMouseWheelComponent>("MouseWheelCom");
    wheelCom->mOnMouseWheelCallbackSystem.Register([this](int wheelDelta) { ScrollListByWheel(wheelDelta); });

    mListContentPanel = mListViewportPanel->CreateChildUIElement<UI::UIImage>("ListContentPanel");
    mListContentPanel->SetWidth(width);
    mListContentPanel->SetUseScissorRect(true);
    mListContentPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
}

void UISearchSelectBox::RebuildItemList()
{
    if (mListContentPanel == nullptr)
        return;

    mListContentPanel->SetPositionLocal(0.0f, 0.0f);

    if (mFilteredItems.empty())
    {
        return;
    }

    for (size_t i = 0; i < mFilteredItems.size(); ++i)
    {
        auto itemButton = GetItemButton(mFilteredItems[i], i);
        itemButton->SetParent(mListContentPanel);
        mItemButtons.push_back(itemButton);
    }

    ShowListPanel();
}

UI::UITextButton *UISearchSelectBox::GetItemButton(const SearchSelectItem &item, size_t itemIndex)
{

    UI::UITextButton *itemButton = nullptr;

    float itemWidth = mTransform.GetSize().x;

    if (mItemButtonPool.empty())
    {
        auto canvas = GetDestCanvas();
        itemButton = canvas->CreateUIElement<UI::UITextButton>("ItemButton");
        itemButton->SetUseScissorRect(true);
        itemButton->SetKeyboardCaptureScope(this);
    }
    else
    {
        itemButton = mItemButtonPool.back();
        itemButton->SetActiveFlag(true);
        mItemButtonPool.pop_back();
    }

    itemButton->SetSize(itemWidth, 30.0f);
    itemButton->mTextComponent->SetFontSize(20.0f);
    itemButton->mTextComponent->SetColor(1, 1, 1);
    itemButton->mTextComponent->SetText(item.mLabel);
    itemButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this, itemIndex](float, float)
                                                                        { OnSelectedItem(itemIndex); });
    return itemButton;
}

void UISearchSelectBox::OnSelectedItem(size_t index)
{

    mSelectingItem = true;

    if (index >= mFilteredItems.size())
        return;

    SearchSelectItem selectItem = mFilteredItems[index];

    mEditBox->SetText(selectItem.mLabel);
    mEditBox->GetTextInputComponent()->ReleaseKeyboardFocus();

    mOnSelectedItemChangedCallbackSystem.ExecuteCallbacks(selectItem.mUniqueNum);

    // ClearVisibleitems();
    HideListPanel();
    RequestClearVisibleItems();

    //    mListViewportPanel->SetActiveFlag(false);
    //   SetHeight(mEditBox->mTransform.GetSize().y);

    mSelectingItem = false;
}

void UISearchSelectBox::ScrollListByWheel(int wheelDelta)
{
    constexpr float scrollStep = 20.0f;

    float dir = wheelDelta > 0 ? 1.0f : -1.0f;

    ScrollList(dir * scrollStep);
}

void UISearchSelectBox::ScrollList(float shift)
{

    if (mListViewportPanel == nullptr || mListContentPanel == nullptr)
        return;

    float maxY = mListContentPanel->mTransform.GetSize().y - mListViewportPanel->mTransform.GetSize().y;
    if (maxY <= 0.0f)
        return;

    float x = mListContentPanel->mTransform.GetLocalPosition().x;
    float y = mListContentPanel->mTransform.GetLocalPosition().y;

    float newY = y;
    newY += shift;

    if (newY <= -maxY)
    {
        newY = -maxY;
    }
    else if (newY >= 0.0f)
    {
        newY = 0.0f;
    }

    mListContentPanel->SetPositionLocal(x, newY);
}

void UISearchSelectBox::OnFinishInput(const std::string &text)
{
    if (mSelectingItem)
        return;

    if (TrySelectItemByLabel(text))
        return;

    if (mEditBox)
        mEditBox->SetText("");

    ClearVisibleitemsImmediately();
}

bool UISearchSelectBox::TrySelectItemByLabel(const std::string &label)
{

    for (const auto &item : mItems)
    {
        if (item.mLabel != label)
            continue;
        SearchSelectItem selectedItem = item;

        if (mEditBox)
            mEditBox->SetText(label);
        // if (mListViewportPanel)
        //{
        //     mListViewportPanel->SetActiveFlag(false);
        // }

        // SetHeight(mEditBox->mTransform.GetSize().y);
        HideListPanel();

        mOnSelectedItemChangedCallbackSystem.ExecuteCallbacks(selectedItem.mUniqueNum);
        return true;
    }

    return false;
}

void UISearchSelectBox::FilterItems(const std::string &text)
{
    mFilteredItems.clear();

    if (text.empty())
    {
        return;
    }

    for (auto item : mItems)
    {

        if (!item.mLabel.starts_with(text))
            continue;

        mFilteredItems.push_back(item);
    }
}

void UISearchSelectBox::ClearVisibleitemsImmediately()
{

    for (auto itemButton : mItemButtons)
    {
        itemButton->SetActiveFlag(false);
        itemButton->SetParent(nullptr);
        itemButton->mUIButtonComponent->mButtonClickCallbackSystem.UnRegisterAll();
        mItemButtonPool.push_back(itemButton);
    }

    mItemButtons.clear();
    mFilteredItems.clear();

    HideListPanel();
}

void UISearchSelectBox::OnSearchTextChanged(const std::string &text)
{
    ClearVisibleitemsImmediately();
    FilterItems(text);
    if (mFilteredItems.empty())
    {
        HideListPanel();
        return;
    }

    RebuildItemList();
}

void UISearchSelectBox::ClearItemButtons() {}

void UISearchSelectBox::RequestClearVisibleItems()
{

    mPendingClearVisibleItems = true;
}

void UISearchSelectBox::HideListPanel()
{

    if (mListViewportPanel)
    {
        mListViewportPanel->SetActiveFlag(false);
    }

    if (mListContentPanel)
        mListContentPanel->SetPositionLocal(0.0f, 0.0f);

    SetHeight(GetEditBoxHeight());
}

void UISearchSelectBox::ShowListPanel()
{

    if (mListViewportPanel)
    {
        mListViewportPanel->SetActiveFlag(true);
    }

    float y = GetEditBoxHeight() + (mListViewportPanel ? mListViewportPanel->mTransform.GetSize().y : 0.0f);
    SetHeight(y);
}

float UISearchSelectBox::GetEditBoxHeight() const
{
    return mEditBox ? mEditBox->mTransform.GetSize().y : 30.0f;
}
