
#include <UISystem/UIButtonComponent.h>
#include <UiSystem/UIElement.h>
UI::UIButtonComponent::UIButtonComponent() : mHover(false), mPress(false) {}

UI::UIButtonComponent::~UIButtonComponent() {}

void UI::UIButtonComponent::Update(float deltaTime) {}

int UI::UIButtonComponent::IsPointInside(float x, float y) const
{
    return IsPointInsideDefault(x, y);
}

void UI::UIButtonComponent::UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                                  bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{
}

// void UI::UIButtonComponent::RegisterOnClickCallback(void *data, void (*onClickCallback)(void *))
//{
//     mData = data;
//     mOnClickCallback = onClickCallback;
// }

void UI::UIButtonComponent::OnHover(int x, int y)
{

    mHover = true;
}
void UI::UIButtonComponent::OnReleaseHover()
{

    mHover = false;
}

void UI::UIButtonComponent::OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) {}

void UI::UIButtonComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                        bool &bConsume)
{

    mPress = true;
    RequestMouseCaptureInput();
}

void UI::UIButtonComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                      bool &bConsume)
{

    ReleaseMouseCaptureInput();
    if (mPress && mHover)
    {
        // TODO
        // Callback 호출
        mButtonClickCallbackSystem.ExecuteCallbacks(worldPosX, worldPosY);
        mPress = false;
    }
}

bool UI::UIButtonComponent::IsHovered() const
{
    return mHover;
}

void UI::UIButtonComponent::OnChangeHoverPart(int before, int after) {}
