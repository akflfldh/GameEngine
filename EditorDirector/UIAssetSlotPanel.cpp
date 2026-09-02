#include "UIAssetSlotPanel.h"
#include <EditorDirector/UIDropTargetComponent.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UIUtility.h>

UIAssetSlotPanel::UIAssetSlotPanel() {}

UIAssetSlotPanel::~UIAssetSlotPanel() {}

void UIAssetSlotPanel::OnBegin()
{
    UI::UIElement::OnBegin();

    float width = 500.0f;
    float height = 300.0f;
    SetSize(width, height);

    mBackgroundPanel = CreateChildUIElement<UI::UIImage>("BackgroundImagePanel");
    mBackgroundPanel->SetSize(width, height);

    float posX = 20.0f;
    float posY = 10.0f;

    mTagText = CreateChildUIElement<UI::UIText>("TagText");
    mTagText->SetFontSize(25.0f);
    mTagText->SetPositionLocal(posX, posY);
    mTagText->SetSize(150.0f, 30.0f);
    mTagText->SetClipingMode(UI::EUITextClipingMode::eEllipsis);
    mTagText->SetOverflowMode(UI::EUITextOverflowMode::eOverflow);
    mTagText->SetTextColor({1, 1, 1, 1});

    // asset slot image panel
    posY = UI::UIUtility::ShiftPosY(posY, mTagText, 10.0f);

    mAssetSlotImagePanel = CreateChildUIElement<UI::UIImage>("AssetSlotImagePanel");
    mAssetSlotImagePanel->SetSize(150.0f, 150.0f);
    mAssetSlotImagePanel->SetPositionLocal(posX, posY);
    mAssetSlotImagePanel->SetColor(0.4f, 0.4f, 0.4f);
    mAssetSlotImagePanel->UseTexture(true);
    UIDropTargetComponent *dropTargetCom =
        mAssetSlotImagePanel->CreateUIComponent<UIDropTargetComponent>("DropTargetCom");

    if (dropTargetCom)
    {
        dropTargetCom->SetDragDropPayloadType(EDragDropType::eAssetTexture);
        dropTargetCom->mOnDroppedPayloadCallbackSystem.Register([this](const DragPayload &payload)
                                                                { OnDroppedPayload(payload); });
    }
}

void UIAssetSlotPanel::SetTagText(const std::string &text)
{

    if (mTagText)
    {
        mTagText->SetText(text);
    }
}

void UIAssetSlotPanel::OnDroppedPayload(const DragPayload &payload)
{

    if (payload.mType != mPayloadType)
    {
        return;
    }
    // callback

    mOnDroppedAssetCallbackSystem.ExecuteCallbacks(payload.mAssetID);
}

void UIAssetSlotPanel::SetDragPayloadType(EDragDropType payloadType)
{

    mPayloadType = payloadType;
}

void UIAssetSlotPanel::SetSlotImage(CoreAsset::Texture *texture)
{

    if (mAssetSlotImagePanel)
    {
        mAssetSlotImagePanel->mImageCom->SetTexture(texture);
    }
}
void UIAssetSlotPanel::OnTransformChanged(UI::ETransformChangeType type)
{

    UIElement::OnTransformChanged(type);

    if (type == UI::ETransformChangeType::eAll || type == UI::ETransformChangeType::eSize)
    {

        if (mBackgroundPanel)
        {
            mBackgroundPanel->SetWidth(GetWidth());
        }
    }
}

void UIAssetSlotPanel::SetBackgroundColor(float r, float g, float b)
{

    if (mBackgroundPanel)
    {
        mBackgroundPanel->SetColor(r, g, b);
    }
}