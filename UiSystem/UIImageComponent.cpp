#include "UiSystem/UIImageComponent.h"
#include "UiSystem/UIElement.h"
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Texture.h>
#include <CoreAsset/UIMaterialManager.h>
#include <Logger/Logger.h>
#include <UiSystem/IUIRenderProxyManager.h>
#include <UiSystem/UIType.h>
#include <assert.h>

UI::UIImageComponent::UIImageComponent() : mUseTexture(false)
{
    SetTexture(nullptr);
}

UI::UIImageComponent::~UIImageComponent() {}

void UI::UIImageComponent::SetTexture(CoreAsset::Texture *texture)
{
    if (mUseTexture == false)
    {
        texture = static_cast<CoreAsset::Texture *>(
            CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::Texture>("/Engine/DefaultWhite").Get());
    }

    CoreAsset::UIMaterialManager *uiMaterialManager = CoreAsset::UIMaterialManager::GetInstance();

    CoreAsset::Material *mat = uiMaterialManager->GetOrCreateDefaultMaterial(texture); // Default텍스처사용

    if (mat == nullptr)
    {
        LOG_MESSAGE_CRITICAL("UIRenderableComponent", "머터리얼 설정실패");
        assert(0);
    }

    mMeshComponent.mUIMaterial = mat;
}

void UI::UIImageComponent::SetTexture(const std::string &path)
{

    auto assetManager = CoreAsset::AssetManager::GetInstance();
    auto pTexture = assetManager->GetAsset<CoreAsset::Texture>(path.c_str());

    CoreAsset::Texture *texture = static_cast<CoreAsset::Texture *>(pTexture.Get());

    if (pTexture.Get() == nullptr)
    {
        texture =
            static_cast<CoreAsset::Texture *>(assetManager->GetAsset<CoreAsset::Texture>("/Engine/DefaultGray").Get());
    }

    SetTexture(texture);
}

void UI::UIImageComponent::SetTexture(CoreAsset::AssetID id)
{

    auto texture = static_cast<CoreAsset::Texture *>(
        CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::Texture>(id).Get());

    SetTexture(texture);
}

void UI::UIImageComponent::SetTemplateMaterial(CoreAsset::AssetID texID)
{
    mMeshComponent.mUIMaterial = CoreAsset::UIMaterialManager::GetInstance()->GetOrCreateDefaultMaterial(texID);
}

size_t UI::UIImageComponent::GetVertexNum() const
{
    return 4;
}

uint32_t UI::UIImageComponent::GetVertices(UIVertex *oUIVertices) const
{

    if (oUIVertices == nullptr)
    {
        return 4;
    }

    UIElement *uiElement = GetOwnerUIElement();
    glm::vec2 vertices[4];
    uiElement->mTransform.GetQuadWorldPoints(vertices);

    uint32_t color = 0;
    if (mUseTexture == true)
    {
        color = UIColorUtility::PackColor({1.0f, 1.0f, 1.0f, 1.0f});
    }
    else
    {
        color = UIColorUtility::PackColor({mColorR, mColorG, mColorB, 1.0f});
    }

    for (int i = 0; i < 4; ++i)
    {
        oUIVertices[i].mPos[0] = vertices[i].r;
        oUIVertices[i].mPos[1] = vertices[i].g;
        oUIVertices[i].mColor = color;
        // oUIVertices[i].mColor = mMeshComponent.mColor;
    }
    oUIVertices[0].mTex[0] = 0;
    oUIVertices[0].mTex[1] = 0;

    oUIVertices[1].mTex[0] = 1;
    oUIVertices[1].mTex[1] = 0;

    oUIVertices[2].mTex[0] = 1;
    oUIVertices[2].mTex[1] = 1;

    oUIVertices[3].mTex[0] = 0;
    oUIVertices[3].mTex[1] = 1;

    return 4;
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

void UI::UIImageComponent::UseTexture()
{

    mUseTexture = true;
}

void UI::UIImageComponent::SetColor(float r, float g, float b)
{

    mColorR = r;
    mColorG = g;
    mColorB = b;
}

void UI::UIImageComponent::SetColor(glm::vec3 color)
{

    mColorR = color.r;
    mColorG = color.g;
    mColorB = color.b;
}

void UI::UIImageComponent::SetColor(const UI::UIColor &color)
{

    mColorR = color.mR;
    mColorG = color.mG;
    mColorB = color.mB;
}

glm::vec3 UI::UIImageComponent::GetColor() const
{
    return {mColorR, mColorG, mColorB};
}

void UI::UIImageComponent::NotUseTexture()
{

    mUseTexture = false;
    SetTexture(nullptr);
}
