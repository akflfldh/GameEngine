#include "UIBoolPanel.h"
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
UIBoolPanel::UIBoolPanel() : mTagText(nullptr), mCheckButton(nullptr) {}

UIBoolPanel::~UIBoolPanel() {}

void UIBoolPanel::OnBegin()
{

    UI::UIImage::OnBegin();

    SetHeight(40.0f);

    mTagText = CreateChildUIElement<UI::UIText>("TagText");
    mTagText->SetHeight(30.0f);
    mTagText->SetPositionLocal(20, 3);
    mTagText->SetTextColor({0, 0, 0});
    mCheckButton = CreateChildUIElement<UI::UIButton>("CheckButton");

    mCheckButton->SetSize(20, 20);
    mCheckButton->mUIImageComponent->SetTexture("Engine/CheckBox");
    mCheckButton->mUIImageComponent->NotUseTexture();
    mCheckButton->mUIImageComponent->SetColor(1.0F, 1.0F, 1.0F);

    mCheckButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            mCurrentValue = !mCurrentValue;
            UpdateVisitualState();
            mOnValueChanged.ExecuteCallbacks(mCurrentValue);
        });

    if (mTagTextStr.empty() == false)
        SetTagText(mTagTextStr);

    UpdateVisitualState();
}

void UIBoolPanel::Update(float deltaTime)
{

    UI::UIImage::Update(deltaTime);
}

void UIBoolPanel::SetTagText(const std::string &tag)
{

    mTagTextStr = tag;

    if (mTagText == nullptr || mCheckButton == nullptr)
        return;

    mTagText->SetText(tag);
    int tagTextWidth = tag.size() * 10.0f;
    mTagText->SetWidth(tagTextWidth);
    mCheckButton->SetPositionLocal(mTagText->mTransform.GetLocalPosition().x + tagTextWidth + 40.0f, 10);
}

void UIBoolPanel::SetCheckValue(bool value, bool bNotify)
{

    if (mCurrentValue == value)
        return;

    mCurrentValue = value;
    if (bNotify)
        mOnValueChanged.ExecuteCallbacks(mCurrentValue);

    if (mCheckButton == nullptr)
        return;

    UpdateVisitualState();
}

void UIBoolPanel::UpdateVisitualState()
{

    if (mCurrentValue)
    {
        mCheckButton->mUIImageComponent->UseTexture();
        mCheckButton->mUIImageComponent->SetTexture("Engine/CheckBox");
    }
    else
    {
        mCheckButton->mUIImageComponent->NotUseTexture();
    }
}
