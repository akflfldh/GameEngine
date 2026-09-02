#include "UiSystem/UIRenderableComponent.h"

#include "UiSystem/UIElement.h"
#include "UiSystem/UIResizeGizmoRenderableComponent.h"
#include <CoreAsset/UIMaterialManager.h>
#include <Logger/Logger.h>
#include <UiSystem/UICanvas.h>
#include <assert.h>
UI::UIRenderableComponent::UIRenderableComponent()
    : mRenderActiveState(true), mRenderProxy(std::make_unique<UIRenderProxy>())
{
}

UI::UIRenderableComponent::~UIRenderableComponent() {}

void UI::UIRenderableComponent::Update(float deltaTime) {}

const UI::UIMeshComponent &UI::UIRenderableComponent::GetUIMeshComponentRef() const
{
    return mMeshComponent;
}

const UI::UIMeshComponent *UI::UIRenderableComponent::GetUIMeshComponentPtr() const
{
    return &mMeshComponent;
}

void UI::UIRenderableComponent::SetColor(const glm::vec4 &color)
{
    mMeshComponent.mColor = color;
}

void UI::UIRenderableComponent::SetActiveState(bool state)
{

    mRenderActiveState = state;

    UIElement *element = GetOwnerUIElement();
    if (element)
    {
        auto canvas = element->GetDestCanvas();

        if (canvas)
        {
            canvas->MarkDirty();
        }
    }
}
bool UI::UIRenderableComponent::GetActiveState() const
{
    return mRenderActiveState;
}

void UI::UIRenderableComponent::OnOwnerAddedToCavas()
{
    IUIRenderProxyManager *RenderProxyManager = IUIRenderProxyManager::GetInstance();

    if (RenderProxyManager)
    {
        mRenderProxy->mCanvas = GetOwnerUIElement()->GetDestCanvas();
        mRenderProxy->mRenderableComponent = this;

        //  RenderProxyManager->RegisterRenderProxy(mRenderProxy.get());
    }
}

UI::UIRenderProxy *UI::UIRenderableComponent::GetRenderProxy() const
{
    return mRenderProxy.get();
}
