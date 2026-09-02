#include "UIMouseDragComponent.h"

UI::UIMouseDragComponent::UIMouseDragComponent() {}
UI::UIMouseDragComponent::~UIMouseDragComponent() {}

int UI::UIMouseDragComponent::IsPointInside(float x, float y) const
{
    return IsPointInsideDefault(x, y);
}

void UI::UIMouseDragComponent::Update(float deltaTime) {}

bool UI::UIMouseDragComponent::IsHovered() const
{
    return mHover;
}

void UI::UIMouseDragComponent::OnHover(int x, int y)
{

    mHover = true;
    mOnHoverCallbackSystem.ExecuteCallbacks();
}
void UI::UIMouseDragComponent::OnReleaseHover()
{

    if (!mPressed && !mDragging)
        mOnReleaseHoverCallbackSystem.ExecuteCallbacks();

    mHover = false;
}

void UI::UIMouseDragComponent::OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY)
{

    if (mPressed)
    {

        if (!mDragging)
        {
            CoreMath::Vector2 delta = mStartPosition - CoreMath::Vector2{worldPosX, worldPosY};

            if (delta.LengthSquared() > 3.0f)
            {
                mDragging = true;
                mPrevPosition = CoreMath::Vector2{worldPosX, worldPosY};
                mOnDragStartedCallbackSystem.ExecuteCallbacks();
            }
        }

        if (mDragging)
        {

            UI::UIMouseDragContext context;
            context.mDeltaX = mPrevPosition.X - worldPosX;
            context.mDeltaY = -1.0f * (mPrevPosition.Y - worldPosY);

            mPrevPosition = CoreMath::Vector2{worldPosX, worldPosY};

            mOnDraggedCallbackSystem.ExecuteCallbacks(context);
        }
    }
}
// virtual void OnMouseClick(const Quad::InputData &inputData, bool &bConsume) override;
void UI::UIMouseDragComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                           bool &bConsume)
{
    if (mHover)
    {
        mStartPosition = {worldPosX, worldPosY};
        RequestMouseCaptureInput();
        mPressed = true;

        mDragging = false;
        bConsume = true;
    }
}
void UI::UIMouseDragComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                         bool &bConsume)
{

    if (mDragging)
    {
        mOnDragEndededCallbackSystem.ExecuteCallbacks();
    }
    mDragging = false;

    if (mPressed)
        ReleaseMouseCaptureInput();

    mPressed = false;
    bConsume = true;
}