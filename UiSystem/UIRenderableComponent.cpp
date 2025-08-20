#include "UiSystem/UIRenderableComponent.h"

#include "UiSystem/UIElement.h"
#include "UiSystem/UIResizeGizmoRenderableComponent.h"
#include <CoreAsset/UIMaterialManager.h>
#include <Logger/Logger.h>
#include <assert.h>
UI::UIRenderableComponent::UIRenderableComponent() : mRenderActiveState(true) {}

UI::UIRenderableComponent::~UIRenderableComponent() {}

void UI::UIRenderableComponent::Update() {}

const UI::UIMeshComponent &UI::UIRenderableComponent::GetUIMeshComponentRef() const
{
    return mMeshComponent;
}

const UI::UIMeshComponent *UI::UIRenderableComponent::GetUIMeshComponentPtr() const
{
    return &mMeshComponent;
}

void UI::UIRenderableComponent::SetColor(const CoreMath::Vector4 &color)
{
    mMeshComponent.mColor = color;
}

void UI::UIRenderableComponent::SetActiveState(bool state)
{

    mRenderActiveState = state;
}
bool UI::UIRenderableComponent::GetActiveState() const
{
    return mRenderActiveState;
}
