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
void UI::UIImage::SetColor(glm::vec3 color)
{

    SetColor(color.r, color.g, color.b);
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

void UI::UIImage::UseTexture(bool flag)
{

    if (flag)
        mImageCom->UseTexture();
    else
        mImageCom->NotUseTexture();
}
