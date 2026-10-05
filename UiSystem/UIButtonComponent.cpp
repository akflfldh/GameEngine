
#include <UISystem/UIButtonComponent.h>
#include <UiSystem/UIElement.h>
UI::UIButtonComponent::UIButtonComponent() : mHover(false), mPress(false), mSelected(false) {}

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
    auto ownerUIElement = GetOwnerUIElement();

    if (ownerUIElement)
    {
        ownerUIElement->DirtyVisualStyle();
    }
}
void UI::UIButtonComponent::OnReleaseHover()
{

    mHover = false;
    auto ownerUIElement = GetOwnerUIElement();

    if (ownerUIElement)
    {
        ownerUIElement->DirtyVisualStyle();
    }
}

void UI::UIButtonComponent::OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) {}

void UI::UIButtonComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                        bool &bConsume)
{
    const auto downState = mTriggerButton == EUIMouseButton::eLeft ? EInputState::eMouseLButtonDown
                                                                  : EInputState::eMouseRButtonDown;
    if (!(inputData.mInputState & downState))
        return;

    if (mPress)
    {
        return; // 이미눌름
    }

    mPress = true;
    mPressedButton = mTriggerButton;
    RequestMouseCaptureInput();
}

void UI::UIButtonComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                      bool &bConsume)
{
    const auto upState = mPressedButton == EUIMouseButton::eLeft ? EInputState::eMouseLButtonUp
                                                               : EInputState::eMouseRButtonUp;
    // 다른 버튼의 Up이나 선행 Down이 없는 Up은 현재 캡처를 해제하지 않는다.
    if (!mPress || !(inputData.mInputState & upState))
        return;

    const bool shouldClick = mPress && mHover;

    // 영역 밖에서 놓아도 press는 종료하며, 콜백이 UI 상태를 바꾸기 전에 입력 상태를 정리한다.
    mPress = false;
    ReleaseMouseCaptureInput();
    if (shouldClick)
    {
        mButtonClickCallbackSystem.ExecuteCallbacks(worldPosX, worldPosY);
    }
}

bool UI::UIButtonComponent::IsHovered() const
{
    return mHover;
}

void UI::UIButtonComponent::SetTriggerButton(EUIMouseButton button)
{
    mTriggerButton = button;
}

UI::EUIMouseButton UI::UIButtonComponent::GetTriggerButton() const
{
    return mTriggerButton;
}

void UI::UIButtonComponent::OnChangeHoverPart(int before, int after) {}

bool UI::UIButtonComponent::IsSelected() const
{
    return mSelected;
}

void UI::UIButtonComponent::SetSelected(bool state)
{

    if (mSelected == state)
        return;

    mSelected = state;

    auto owner = GetOwnerUIElement();

    if (owner)
    {
        owner->DirtyVisualStyle();
    }
}
