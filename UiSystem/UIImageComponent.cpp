#include "UiSystem/UIImageComponent.h"
#include "UiSystem/UIElement.h"
#include <CoreAsset/UIMaterialManager.h>
#include <Logger/Logger.h>
#include <assert.h>

UI::UIImageComponent::UIImageComponent()
{
    SetTexture(nullptr);
}

UI::UIImageComponent::~UIImageComponent() {}

void UI::UIImageComponent::SetTexture(CoreAsset::Texture *texture)
{

    CoreAsset::UIMaterialManager *uiMaterialManager = CoreAsset::UIMaterialManager::GetInstance();
    CoreAsset::Material *mat = uiMaterialManager->GetOrCreateDefaultMaterial(texture); // Default텍스처사용

    if (mat == nullptr)
    {
        LOG_MESSAGE_CRITICAL("UIRenderableComponent", "머터리얼 설정실패");
        assert(0);
    }

    mMeshComponent.mUIMaterial = mat;
}

void UI::UIImageComponent::SetTemplateMaterial(CoreAsset::AssetID texID)
{
    mMeshComponent.mUIMaterial = CoreAsset::UIMaterialManager::GetInstance()->GetOrCreateDefaultMaterial(texID);
}

size_t UI::UIImageComponent::GetVertexNum() const
{
    return 4;
}

void UI::UIImageComponent::GetVertices(UIVertex *oUIVertices) const
{

    UIElement *uiElement = GetOwnerUIElement();
    CoreMath::Vector2 vertices[4];
    uiElement->mTransform.GetQuadWorldPoints(vertices);

    for (int i = 0; i < 4; ++i)
    {
        oUIVertices[i].mPos = vertices[i];
        oUIVertices[i].mColor = mMeshComponent.mColor;
    }
    oUIVertices[0].mTex = {0, 0};
    oUIVertices[1].mTex = {1, 0};
    oUIVertices[2].mTex = {1, 1};
    oUIVertices[3].mTex = {0, 1};
}

size_t UI::UIImageComponent::GetIndexNum() const
{
    return 6;
}

void UI::UIImageComponent::GetIndices(uint32_t *oIndices) const
{

    // 윗삼각1
    oIndices[0] = 0;
    oIndices[1] = 1;
    oIndices[2] = 3;

    // 아랫삼각2
    oIndices[3] = 3;
    oIndices[4] = 1;
    oIndices[5] = 2;
}
