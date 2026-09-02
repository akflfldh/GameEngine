#include "UiSystem/IUIComponent.h"
#include "UiSystem/UIComponentFactory.h"
#include <UiSystem/UIElement.h>

UI::IUIComponent::IUIComponent() : mDepthValue(0), mMouseHitSectionNum(0), mOwnerUIElement(nullptr) {}

UI::IUIComponent::~IUIComponent() {}

void UI::IUIComponent::Begin()
{

    OnBegin();
}

const char *UI::IUIComponent::GetComponentName() const
{
    return mName;
}

UI::UIElement *UI::IUIComponent::GetOwnerUIElement() const
{
    return mOwnerUIElement;
}

void UI::IUIComponent::SetOwnerUIElement(UIElement *owner)
{

    mOwnerUIElement = owner;
    if (owner->GetDestCanvas())
    {
        OnOwnerAddedToCavas();
    }
}

void UI::IUIComponent::SetUIComponentHandle(const UIComponentHandle &handle)
{

    mUIComponentHandle = handle;
}

int UI::IUIComponent::IsPointInside(float x, float y) const
{

    return 0;
}

void UI::IUIComponent::UpdateMouseInputEvent(const UI::UIManagerMouseInputContext &mouseInputContext,
                                             bool &captureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{
    return;
}

void UI::IUIComponent::SetDepthValue(uint32_t depth)
{
    mDepthValue = depth;
}

uint32_t UI::IUIComponent::GetDepthValue() const
{
    return mDepthValue;
}

const UI::UIComponentHandle &UI::IUIComponent::GetHandle() const
{

    return mUIComponentHandle;
}

void UI::IUIComponent::OnOwnerAddedToCavas() {}

void UI::IUIComponent::HandleInput(const Quad::RawInputData &inputData, bool &bConsume) {}
void UI::IUIComponent::SetInputEnable(bool flag)
{
    mInputEnable = flag;
}
bool UI::IUIComponent::GetInputEnable() const
{
    return mInputEnable;
}
void UI::IUIComponent::OnHover(int x, int y) {

    // TO DO
    // 이거는 부모의 영역이 일단 hover일때 호출된것

    // 각 uicomponent에따라 이곳에서 혹은 mouseMover일때 실제 hover판정을 할것디ㅏ.

};
void UI::IUIComponent::OnReleaseHover()
{

    // TO DO
    //  마우스가 부모 UIElement의 영역에서 벗어남,

    // 확실히 hover 해제
}

void UI::IUIComponent::OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) {}
void UI::IUIComponent::OnMouseClick(const Quad::RawInputData &inputData, bool &bConsume) {}
void UI::IUIComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                   bool &bConsume)
{
}
void UI::IUIComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY, bool &bConsume)
{
}
void UI::IUIComponent::OnKeyDown(const Quad::EKeyCode &inputData, bool &bConsume) {}

void UI::IUIComponent::OnMouseWheel(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                    bool &bConsume)
{
}

void UI::IUIComponent::OnKeyUp(const Quad::EKeyCode &key, bool &bConsume) {}

void UI::IUIComponent::OnPreviewMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                          bool &bSteal)
{
}

void UI::IUIComponent::OnPreviewMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                        bool &bSteal)
{
}

void UI::IUIComponent::OnChar(uint32_t ch, bool &bConsume) {}

bool UI::IUIComponent::IsHovered() const
{

    return false;
}

void UI::IUIComponent::OnChangeHoverPart(int before, int after)
{
    mMouseHitSectionNum = after;
}

int UI::IUIComponent::GetHoverPart() const
{
    return mMouseHitSectionNum;
}

void UI::IUIComponent::SetHoverPart(int num)
{

    mMouseHitSectionNum = num;
}

void UI::IUIComponent::RequestMouseCaptureInput()
{

    if (mOwnerUIElement)
    {
        mOwnerUIElement->RequestMouseCaptureInput(this);
    }
}
void UI::IUIComponent::ReleaseMouseCaptureInput()
{

    if (mOwnerUIElement)
    {
        mOwnerUIElement->ReleaseMouseCaptureInput();
    }
}
void UI::IUIComponent::RequestKeyboardCaptureInput()
{

    if (mOwnerUIElement)
    {
        mOwnerUIElement->RequestKeyboardCaptureInput(this);
    }
}
void UI::IUIComponent::ReleaseKeyboardCaptureInput()
{

    if (mOwnerUIElement)
    {
        mOwnerUIElement->ReleaseKeyboardCaptureInput();
    }
}

void UI::IUIComponent::OnSetMouseFocus(bool isRequester)
{

    mOnwerHasMouseCapture = true;
}

void UI::IUIComponent::OnLostMouseFocus()
{
    mOnwerHasMouseCapture = false;
}

void UI::IUIComponent::OnSetKeyboardFocus(bool isRequester)
{

    mOwnerHasKeyboardCapture = true;
}

void UI::IUIComponent::OnLostKeyboardFocus()
{

    mOwnerHasKeyboardCapture = false;
}

bool UI::IUIComponent::IsPointInsideDefault(float x, float y) const
{
    UIElement *ownerUIElement = GetOwnerUIElement();

    glm::vec2 vertices[4];
    ownerUIElement->mTransform.GetQuadWorldPoints(vertices);

    float leftX = vertices[0].x;
    float rightX = vertices[1].x;
    float topY = vertices[0].y;
    float bottomY = vertices[2].y;

    if (leftX > x)
        return false;
    if (rightX < x)
        return false;
    if (topY > y)
        return false;
    if (bottomY < y)
        return false;

    return true;
}

void UI::IUIComponent::Serialize(Arch &arch) {}

void UI::IUIComponent::SetPreviewInputListener(bool flag)
{

    mPreviewInputListener = flag;
}

bool UI::IUIComponent::GetPreviewInputListener() const
{
    return mPreviewInputListener;
}
