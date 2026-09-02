#include "UIAssetSlotListPanel.h"
#include <UIAssetSlotPanel.h>
#include <UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIVerticalLayoutComponent.h>
UIAssetSlotListPanel::UIAssetSlotListPanel()
{

    UI::UIVerticalLayoutComponent *verticalLayoutCom =
        CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
}

UIAssetSlotListPanel::~UIAssetSlotListPanel() {}

void UIAssetSlotListPanel::OnBegin()
{

    UI::UIElement::OnBegin();

    float width = 600.0f;
    float height = 500.0f;

    SetSize(width, height);

    //  auto buttonBackground = CreateChildUIElement<UI::UIElement>("ButtonBackground");
    //  buttonBackground->SetSize(width, 40.0f);

    mAddButton = CreateChildUIElement<UI::UIButton>("AddButton");
    mAddButton->SetSize(30.0f, 30.0f);
    mAddButton->SetPositionLocal(width - mAddButton->GetWidth() - 10.0f, 5.0f);
    mAddButton->mUIImageComponent->UseTexture();
    mAddButton->mUIImageComponent->SetTexture("Engine/plus");

    mAddButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float) { OnClickedAddButton(); });
}

void UIAssetSlotListPanel::ActiveAssetSlotPanels(int num)
{

    if (num > mSlotPanels.size())
    {
        // 추가 생성
        while (mSlotPanels.size() < num)
        {
            AddSlotPanel();
        }
    }

    for (int i = 0; i < num; ++i)
    {
        mSlotPanels[i]->SetActiveFlag(true);
    }

    for (int i = num; i < mSlotPanels.size(); ++i)
    {
        mSlotPanels[i]->SetActiveFlag(false);
    }
}
void UIAssetSlotListPanel::SetTagText(const std::string &text)
{
    mTagText = text;

    for (auto slotPanel : mSlotPanels)
    {
        slotPanel->SetTagText(text);
    }
}

void UIAssetSlotListPanel::OnClickedAddButton()
{

    // Slot 하나 추가
    AddSlotPanel();

    mOnClickedAddButtonCallbackSystem.ExecuteCallbacks();
}

void UIAssetSlotListPanel::AddSlotPanel()
{
    UIAssetSlotPanel *slotPanel = CreateChildUIElement<UIAssetSlotPanel>("AssetSlotPanel");
    mSlotPanels.push_back(slotPanel);
    slotPanel->mOnDroppedAssetCallbackSystem.Register([this, slotPanel](CoreAsset::AssetID assetID)
                                                      { OnDroppedAssetFileItem(assetID, slotPanel); });

    slotPanel->SetTagText(mTagText);
    slotPanel->SetDragPayloadType(mPayloadType);
}

void UIAssetSlotListPanel::OnDroppedAssetFileItem(CoreAsset::AssetID id, UIAssetSlotPanel *panel)
{

    auto it = std::find(mSlotPanels.begin(), mSlotPanels.end(), panel);
    if (it == mSlotPanels.end())
        return;

    size_t index = it - mSlotPanels.begin();

    mOnDroppedAssetCallbackSystem.ExecuteCallbacks(id, index);
}

void UIAssetSlotListPanel::SetDragPayloadType(EDragDropType payloadType)
{

    mPayloadType = payloadType;
    for (auto slotPanel : mSlotPanels)
    {
        slotPanel->SetDragPayloadType(mPayloadType);
    }
}

void UIAssetSlotListPanel::SetAssetSlotImage(CoreAsset::Texture *texture, int index)
{

    if (mSlotPanels.size() <= index)
        return;

    if (mSlotPanels[index])
        mSlotPanels[index]->SetSlotImage(texture);
}

size_t UIAssetSlotListPanel::GetSlotNum() const
{
    return mSlotPanels.size();
}
