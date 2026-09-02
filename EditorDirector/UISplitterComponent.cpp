#include "UISplitterComponent.h"
#include <EditorDirector/UISplitterPanel.h>
#include <UIElement.h>
#include <UiSystem/UIImageComponent.h>

UISplitterComponent::UISplitterComponent() : mHover(false), mPress(false), mSplitterPanel(nullptr) {}

UISplitterComponent::~UISplitterComponent() {}

void UISplitterComponent::SetSplitterPanel(UISplitterPanel *panel)
{

    mSplitterPanel = panel;
}

int UISplitterComponent::IsPointInside(float x, float y) const
{
    return IsPointInsideDefault(x, y);
}

void UISplitterComponent::Update(float deltaTime) {}

void UISplitterComponent::OnHover(int x, int y)
{
    mHover = true;

    auto element = GetOwnerUIElement();

    if (element)
    {
        UI::UIImageComponent *imageCom = nullptr;
        element->GetComponents<UI::UIImageComponent>(&imageCom, 1);

        if (imageCom)
        {
            glm::vec3 color = imageCom->GetColor();
            color *= 0.7f;
            imageCom->SetColor(color);
        }
    }
}

void UISplitterComponent::OnReleaseHover()
{

    mHover = false;

    if (mPress == true)
        return;

    auto element = GetOwnerUIElement();

    if (element)
    {
        UI::UIImageComponent *imageCom = nullptr;
        element->GetComponents<UI::UIImageComponent>(&imageCom, 1);

        if (imageCom)
        {
            glm::vec3 color = imageCom->GetColor();
            color /= 0.7f;
            imageCom->SetColor(color);
        }
    }
}

void UISplitterComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                      bool &bConsume)
{

    if (mHover)
    {
        mPress = true;
        mMouseOffset = worldPosX - GetOwnerUIElement()->mTransform.GetWorldPosition().r;
        GetOwnerUIElement()->RequestMouseCaptureInput(this);
    }
}

void UISplitterComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                    bool &bConsume)
{

    mPress = false;

    if (!mHover)
    {
        auto element = GetOwnerUIElement();

        if (element)
        {
            UI::UIImageComponent *imageCom = nullptr;
            element->GetComponents<UI::UIImageComponent>(&imageCom, 1);

            if (imageCom)
            {
                glm::vec3 color = imageCom->GetColor();
                color /= 0.7f;
                imageCom->SetColor(color);
            }
        }
    }
    GetOwnerUIElement()->ReleaseMouseCaptureInput();
    mMouseOffset = 0;
}

void UISplitterComponent::OnMouseMove(const Quad::RawInputData &inputData, float mouseWorldPosX, float mouseWorldPosY)
{

    if (mPress)
    {

        float worldX = GetOwnerUIElement()->mTransform.GetWorldPosition().r;
        float deltaX = mouseWorldPosX - (worldX + mMouseOffset);

        mSplitterPanel->OnSplitterDragged(deltaX);
    }
}

bool UISplitterComponent::IsHovered() const
{
    return mHover;
}
