#include "UiSystem/UIImage.h"
#include <CoreAsset/AssetPtr.h>
#include <UIImageComponent.h>

UI::UIImage::UIImage()
{

    mImageCom = CreateUIComponent<UI::UIImageComponent>("ImageCom");
}

UI::UIImage::~UIImage() {}

void UI::UIImage::OnBegin()
{

    UI::UIElement::OnBegin();
}

void UI::UIImage::Update(float deltaTime)
{
    UIElement::Update(deltaTime);
}

void UI::UIImage::SetColor(float r, float g, float b)
{
    if (mImageCom)
    {
        mImageCom->SetColor(r, g, b);
    }
}
void UI::UIImage::SetColor(const UI::UIColor &color)
{

    SetColor(color.mR, color.mG, color.mB);
}
glm::vec3 UI::UIImage::GetColor() const
{

    if (mImageCom)
    {
        return mImageCom->GetColor();
    }
}

void UI::UIImage::SetTexture(CoreAsset::AssetID id)
{

    mImageCom->SetTexture(id);
}

void UI::UIImage::SetTexture(const char *path)
{
    mImageCom->SetTexture(path);
}

void UI::UIImage::UseTexture(bool flag)
{

    if (flag)
        mImageCom->UseTexture();
    else
        mImageCom->NotUseTexture();
}

void UI::UIImage::ApplyLayoutStyle(const UIControlStyle &style)
{

    SetHeight(style.mHeight);
}

void UI::UIImage::ApplyVisualStyle(const UIControlStyle &style, EUIVisualState visualState)
{

    if (mImageCom)
    {

        switch (visualState)
        {
        case EUIVisualState::eNormal:
            mImageCom->SetColor(style.mBackgroundColor);
            break;
        case EUIVisualState::eSelected:
            mImageCom->SetColor(style.mSelectedColor);

        case EUIVisualState::eHovered:
            mImageCom->SetColor(style.mHoverColor);
            break;
        }
    }
}