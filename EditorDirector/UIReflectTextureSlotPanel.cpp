#include "UIReflectTextureSlotPanel.h"
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Texture.h>

UIReflectTextureSlotPanel::UIReflectTextureSlotPanel() {}
UIReflectTextureSlotPanel::~UIReflectTextureSlotPanel() {}

void UIReflectTextureSlotPanel::BindTexture(Getter getter, Setter setter)
{

    mGetter = getter;
    mSetter = setter;
}

void UIReflectTextureSlotPanel::RefreshFromSource()
{

    if (!mGetter)
        return;

    CoreAsset::AssetID textureID = mGetter();

    CoreAsset::Texture *texture =
        CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::Texture>(textureID).As<CoreAsset::Texture>();

    SetSlotImage(texture);
}

void UIReflectTextureSlotPanel::OnBegin()
{

    UIAssetSlotPanel::OnBegin();
    SetDragPayloadType(EDragDropType::eAssetTexture);

    mOnDroppedAssetCallbackSystem.Register([this](CoreAsset::AssetID id) { OnDroppedTexture(id); });
}

void UIReflectTextureSlotPanel::OnDroppedTexture(CoreAsset::AssetID textureID)
{

    if (!mSetter)
    {
        return;
    }

    mSetter(textureID);
    RefreshFromSource();
}
