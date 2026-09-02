#include "UIMultiRectComponent.h"
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Texture.h>
#include <CoreAsset/UIMaterialManager.h>
#include <CoreBase/CoreAssert.h>
#include <UiSystem/UIElement.h>
UI::UIMultiRectComponent::UIMultiRectComponent() : mDirty(false), mColorDirty(false)
{

    SetActiveState(false);
}

UI::UIMultiRectComponent::~UIMultiRectComponent() {}

void UI::UIMultiRectComponent::Update(float deltaTime) {}

size_t UI::UIMultiRectComponent::GetVertexNum() const
{

    UpdateVertex();

    UpdateColor();

    return mVertices.size();
}

uint32_t UI::UIMultiRectComponent::GetVertices(UIVertex *oUIVertices) const
{

    UpdateVertex();
    UpdateColor();

    glm::vec2 originalPos = GetOwnerUIElement()->mTransform.GetWorldPosition();

    if (oUIVertices == nullptr)
    {
        return static_cast<uint32_t>(mVertices.size());
    }

    for (size_t i = 0; i < mVertices.size(); ++i)
    {
        oUIVertices[i] = mVertices[i];
        oUIVertices[i].mPos += originalPos; // 여기서 월드 좌표로 변환
    }
    return static_cast<uint32_t>(mVertices.size());
}

size_t UI::UIMultiRectComponent::GetIndexNum() const
{
    UpdateVertex();

    return mIndices.size();
}

void UI::UIMultiRectComponent::GetIndices(uint32_t *oIndices) const
{
    UpdateVertex();
    for (int i = 0; i < mIndices.size(); ++i)
    {
        oIndices[i] = mIndices[i];
    }
}

void UI::UIMultiRectComponent::SetRects(const std::vector<RenderRect> &rects)
{
    mRectList = rects;
    mDirty = true;
}

void UI::UIMultiRectComponent::SetColor(float r, float g, float b, float a)
{

    SetColor({r, g, b, a});
}
void UI::UIMultiRectComponent::SetColor(const UI::UIColor &color)
{

    mColor = color;
    mColorDirty = true;
}

void UI::UIMultiRectComponent::Reset()
{

    mVertices.clear();
    mIndices.clear();
    mDirty = false;
}

void UI::UIMultiRectComponent::OnBegin()
{

    CoreAsset::Texture *texture = static_cast<CoreAsset::Texture *>(
        CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::Texture>("/Engine/DefaultWhite").Get());

    CoreAsset::UIMaterialManager *uiMaterialManager = CoreAsset::UIMaterialManager::GetInstance();

    CoreAsset::Material *mat = uiMaterialManager->GetOrCreateDefaultMaterial(texture); // Default텍스처사용

    CHECK(mat != nullptr, "DefaultWhite Mat fail");

    mMeshComponent.mUIMaterial = mat;
}

void UI::UIMultiRectComponent::UpdateVertex() const
{

    if (!mDirty)
        return;

    mVertices.clear();
    mIndices.clear();

    uint32_t indexOffset = 0;
    uint32_t packedColor = UI::UIColorUtility::PackColor(mColor.mR, mColor.mG, mColor.mB, mColor.mA);
    for (const auto &rect : mRectList)
    {
        UI::UIVertex v[4];
        // top - left
        v[0].mPos = rect.mTopLeftPos;
        v[0].mTex = {0.0f, 0.0f};
        v[0].mColor = packedColor;

        // top - right
        v[1].mPos.x = rect.mBottomRightPos.x;
        v[1].mPos.y = rect.mTopLeftPos.y;
        v[1].mTex = {1.0f, 0.0f};
        v[1].mColor = packedColor;

        // bottom - right
        v[2].mPos = rect.mBottomRightPos;
        v[2].mTex = {1.0f, 1.0f};
        v[2].mColor = packedColor;

        // bottom-left
        v[3].mPos.x = rect.mTopLeftPos.x;
        v[3].mPos.y = rect.mBottomRightPos.y;
        v[3].mTex = {0.0f, 1.0f};
        v[3].mColor = packedColor;

        mVertices.push_back(v[0]);
        mVertices.push_back(v[1]);
        mVertices.push_back(v[2]);
        mVertices.push_back(v[3]);

        // index

        // 0-1-3
        mIndices.push_back(indexOffset + 0);
        mIndices.push_back(indexOffset + 1);
        mIndices.push_back(indexOffset + 3);

        // 3-1-2
        mIndices.push_back(indexOffset + 3);
        mIndices.push_back(indexOffset + 1);
        mIndices.push_back(indexOffset + 2);
        indexOffset += 4;
    }
    mColorDirty = false;
    mDirty = false;
}

void UI::UIMultiRectComponent::UpdateColor() const
{
    if (!mColorDirty)
        return;

    uint32_t packedColor = UI::UIColorUtility::PackColor(mColor.mR, mColor.mG, mColor.mB, mColor.mA);
    for (int i = 0; i < mVertices.size(); ++i)
    {
        mVertices[i].mColor = packedColor;
    }

    mColorDirty = false;
}