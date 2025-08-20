#include "UiSystem/UIImage.h"
#include "UiSystem/UIRenderableComponent.h"
#include <CoreAsset/AssetPtr.h>

UI::UIImage::UIImage(UIElementID id, const std::string &name) : UIElement(id, name) {}

UI::UIImage::~UIImage() {}

void UI::UIImage::Begin()
{
    UIElement::Begin();

    mRenderableComponent = CreateUIComponent<UIRenderableComponent>("RenderCom");
}

void UI::UIImage::Update(float deltaTime)
{
    UIElement::Update(deltaTime);
}
