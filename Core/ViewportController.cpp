#include "Core/ViewportController.h"
#include "SuperController.h"
#include <Utility/Utility.h>
Core::ViewportController::ViewportController() : mDirtyFlag(true) {}

Core::ViewportController::~ViewportController() {}

void Core::ViewportController::SetViewportMode(EViewportMode mode)
{

    mViewportMode = mode;
}

void Core::ViewportController::ConvertToNdc(float &oClientPosX, float &oClientPosY, float viewportOffsetTopLeftX,
                                            float viewportOffsetTopLeftY) const
{

    Render::Viewport viewport = GetViewport();

    oClientPosX = (oClientPosX - (viewport.TopLeftX + viewportOffsetTopLeftX)) / viewport.Width * 2 - 1;
    oClientPosY = (oClientPosY - (viewport.TopLeftY + viewportOffsetTopLeftY)) / viewport.Height * -2 + 1;
}

bool Core::ViewportController::IntersectPoint(int px, int py, float viewportOffsetTopLeftX,
                                              float viewportOffsetTopLeftY)
{

    Render::Viewport viewport = GetViewport();
    return CoreUtility::Utility::IsPointInsideRect((viewport.TopLeftX + viewportOffsetTopLeftX),
                                                   viewport.TopLeftX + viewportOffsetTopLeftX + viewport.Width,
                                                   viewport.TopLeftY + viewportOffsetTopLeftY + viewport.Height,
                                                   viewport.TopLeftY + viewportOffsetTopLeftY, px, py);
}

void Core::ViewportController::Update() const
{

    // 현재 모드에따라 viewport를 계산한다.

    // fixed mode
    // fixed mode는
    if (mViewportMode == EViewportMode::eFixed)
    {
        mViewport.TopLeftX = mFixedContext.mLeft;
        mViewport.TopLeftY = mFixedContext.mTop;
        mViewport.Width = mFixedContext.mWidth;
        mViewport.Height = mFixedContext.mHeight;
    }
    else
    {
        // anchored mode

        UpdateAnchoredModeAxisX();
        UpdateAnchoredModeAxisY();
    }

    // anchor mode

    mDirtyFlag = false;
}

void Core::ViewportController::UpdateAnchoredModeAxisX() const
{

    const ViewportAnchor &leftAnchor = mAnchoredContext.mLeftAnchor;
    const ViewportAnchor &rightAnchor = mAnchoredContext.mRightAnchor;
    int viewportLeft = 0;
    int viewportRight = 0;

    if (leftAnchor.mAnchorOn == false && rightAnchor.mAnchorOn == false)
    {
        // 두 앵커모두 비활성화

        mViewport.TopLeftX = mFixedContext.mLeft;
        mViewport.Width = mFixedContext.mWidth;
        return;
    }

    if (leftAnchor.mAnchorOn == true)
    {
        // left 앵커 활성화
        if (leftAnchor.mAnchorMode == Core::EViewportAnchoredMode::eRelative)
        {
            viewportLeft = mWindowWidth * leftAnchor.mRel;
        }
        else
        {
            viewportLeft = leftAnchor.mPixel;
        }
    }

    if (rightAnchor.mAnchorOn == true)
    {
        // right 앵커 활성화,
        if (rightAnchor.mAnchorMode == Core::EViewportAnchoredMode::eRelative)
        {
            viewportRight = (mWindowWidth * (1.0f - rightAnchor.mRel));
        }
        else
        {
            viewportRight = mWindowWidth - rightAnchor.mPixel;
        }
    }

    // 비활성화는 left,right중 활성화된 앵커가 먼저 계산된후 처리되어야한다. 게산식이 의존하기때문에
    if (leftAnchor.mAnchorOn == false)
    {
        // left앵커 비활성화
        if (mAnchoredContext.mWidth.mAnchorMode == EViewportAnchoredMode::eRelative)
        {
            int viewportWidth = mAnchoredContext.mWidth.mRel * mWindowWidth;
            viewportLeft = viewportRight - viewportWidth;
        }
        else
        {
            viewportLeft = viewportRight - mAnchoredContext.mWidth.mPixel;
        }
    }

    if (rightAnchor.mAnchorOn == false)
    {
        // right앵커 비활성화
        if (mAnchoredContext.mWidth.mAnchorMode == EViewportAnchoredMode::eRelative)
        {
            int viewportWidth = mAnchoredContext.mWidth.mRel * mWindowWidth;
            viewportRight = viewportLeft + viewportWidth;
        }
        else
        {
            viewportRight = viewportLeft + mAnchoredContext.mWidth.mPixel;
        }
    }

    if (viewportLeft >= viewportRight)
        viewportRight = viewportLeft + 1;

    mViewport.TopLeftX = viewportLeft;
    mViewport.Width = viewportRight - viewportLeft;
}

void Core::ViewportController::UpdateAnchoredModeAxisY() const
{

    const ViewportAnchor &topAnchor = mAnchoredContext.mTopAnchor;
    const ViewportAnchor &bottomAnchor = mAnchoredContext.mBottomAnchor;
    int viewportTop = 0;
    int viewportBottom = 0;

    if (topAnchor.mAnchorOn == false && bottomAnchor.mAnchorOn == false)
    {
        // 두 앵커모두 비활성화

        mViewport.TopLeftY = mFixedContext.mTop;
        mViewport.Height = mFixedContext.mHeight;
        return;
    }

    if (topAnchor.mAnchorOn == true)
    {
        // top 앵커 활성화
        if (topAnchor.mAnchorMode == Core::EViewportAnchoredMode::eRelative)
        {
            viewportTop = mWindowHeight * topAnchor.mRel;
        }
        else
        {
            viewportTop = topAnchor.mPixel;
        }
    }

    if (bottomAnchor.mAnchorOn == true)
    {
        // bottom 앵커 활성화,
        if (bottomAnchor.mAnchorMode == Core::EViewportAnchoredMode::eRelative)
        {
            viewportBottom = (mWindowHeight * (1.0f - bottomAnchor.mRel));
        }
        else
        {
            viewportBottom = mWindowHeight - bottomAnchor.mPixel;
        }
    }

    if (topAnchor.mAnchorOn == false)
    {
        // top앵커 비활성화
        if (mAnchoredContext.mHeight.mAnchorMode == EViewportAnchoredMode::eRelative)
        {
            int viewportHeight = mAnchoredContext.mHeight.mRel * mWindowHeight;
            viewportTop = viewportBottom - viewportHeight;
        }
        else
        {
            viewportTop = viewportBottom - mAnchoredContext.mHeight.mPixel;
        }
    }

    if (bottomAnchor.mAnchorOn == false)
    {
        // bottom앵커 비활성화
        if (mAnchoredContext.mHeight.mAnchorMode == EViewportAnchoredMode::eRelative)
        {
            int viewportHeight = mAnchoredContext.mHeight.mRel * mWindowHeight;
            viewportBottom = viewportTop + viewportHeight;
        }
        else
        {
            viewportBottom = viewportTop + mAnchoredContext.mHeight.mPixel;
        }
    }

    if (viewportTop >= viewportBottom)
        viewportBottom = viewportTop + 1;

    mViewport.TopLeftY = viewportTop;
    mViewport.Height = viewportBottom - viewportTop;
}

float Core::ViewportController::ClampRelAnchor(float relValue)
{
    return relValue < 0 ? 0 : (relValue > 1.0f ? 1.0f : relValue);
}

int Core::ViewportController::ClampPixelHeightValue(int value)
{
    return value < 0 ? 0 : (value > mWindowHeight ? mWindowHeight : value);
}

int Core::ViewportController::ClampPixelWidthValue(int value)
{
    return value < 0 ? 0 : (value > mWindowWidth ? mWindowWidth : value);
}

void Core::ViewportController::UpdateWindowSize(uint32_t width, uint32_t height)
{

    mWindowWidth = width;
    mWindowHeight = height;
    if (mWindowHeight == 2250.0f)
    {
        int a = 2;
    }

    mDirtyFlag = true;
    // Update();
    //     mDirtyFlag = true;
    //  update();

    // 이때만 다시계산하는것
    // 따라서 외부에서 window가 변하면 적절히 호출해줘야 올바르게 viewport가 계산된다.
}
Render::Viewport Core::ViewportController::GetViewport() const
{

    if (mDirtyFlag == true)
    {
        Update();
    }

    return mViewport;
}

float Core::ViewportController::GetAspect() const
{
    const auto &viewport = GetViewport();

    return viewport.Width / viewport.Height;
}

void Core::ViewportController::SetFixedPos(int left, int top)
{

    mFixedContext.mLeft = left;
    mFixedContext.mTop = top;

    mDirtyFlag = true;
}

void Core::ViewportController::SetFixedSize(int width, int height)
{

    mFixedContext.mWidth = width;
    mFixedContext.mHeight = height;

    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorLeftState(bool state)
{

    mAnchoredContext.mLeftAnchor.mAnchorOn = state;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorRightState(bool state)
{
    mAnchoredContext.mRightAnchor.mAnchorOn = state;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorTopState(bool state)
{
    mAnchoredContext.mTopAnchor.mAnchorOn = state;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorBottomState(bool state)
{
    mAnchoredContext.mBottomAnchor.mAnchorOn = state;

    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorLeftMode(EViewportAnchoredMode mode)
{

    mAnchoredContext.mLeftAnchor.mAnchorMode = mode;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorRightMode(EViewportAnchoredMode mode)
{
    mAnchoredContext.mRightAnchor.mAnchorMode = mode;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorTopMode(EViewportAnchoredMode mode)
{
    mAnchoredContext.mTopAnchor.mAnchorMode = mode;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorBottomMode(EViewportAnchoredMode mode)
{
    mAnchoredContext.mBottomAnchor.mAnchorMode = mode;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorLeftRelValue(float value)
{

    mAnchoredContext.mLeftAnchor.mRel = ClampRelAnchor(value);
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorRightRelValue(float value)
{
    mAnchoredContext.mRightAnchor.mRel = ClampRelAnchor(value);
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorTopRelValue(float value)
{
    mAnchoredContext.mTopAnchor.mRel = ClampRelAnchor(value);
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorBottomRelValue(float value)
{
    mAnchoredContext.mBottomAnchor.mRel = ClampRelAnchor(value);
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorLeftPixelValue(uint32_t value)
{
    mAnchoredContext.mLeftAnchor.mPixel = value;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorRightPixelValue(uint32_t value)
{
    mAnchoredContext.mRightAnchor.mPixel = value;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorTopPixelValue(uint32_t value)
{
    mAnchoredContext.mTopAnchor.mPixel = value;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorBottomPixelValue(uint32_t value)
{
    mAnchoredContext.mBottomAnchor.mPixel = value;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorWidthSizeMode(EViewportAnchoredMode mode)
{

    mAnchoredContext.mWidth.mAnchorMode = mode;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorHeightSizeMode(EViewportAnchoredMode mode)
{
    mAnchoredContext.mHeight.mAnchorMode = mode;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorWidthRelValue(float value)
{
    mAnchoredContext.mWidth.mRel = ClampRelAnchor(value);
    ;
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorHeightRelValue(float value)
{

    mAnchoredContext.mHeight.mRel = ClampRelAnchor(value);
    mDirtyFlag = true;
}

void Core::ViewportController::SetAnchorWidthPixelValue(uint32_t value)
{

    mAnchoredContext.mWidth.mPixel = value;
    mDirtyFlag = true;
}
void Core::ViewportController::SetAnchorHeightPixelValue(uint32_t value)
{

    mAnchoredContext.mHeight.mPixel = value;
    mDirtyFlag = true;
}

Core::EViewportMode Core::ViewportController::GetViewportMode() const
{
    return mViewportMode;
}

std::pair<uint32_t, uint32_t> Core::ViewportController::GetWindowSize() const
{
    return {mViewport.Width, mViewport.Height};
}

Core::WorkSpace *Core::SuperController::GetWorkSpace() const
{

    return mWorkSpace;
}

void Core::SuperController::SetWorkSpace(Core::WorkSpace *workspace)
{

    mWorkSpace = workspace;
}

int Core::ViewportController::GetFixedLeft() const
{
    return mFixedContext.mLeft;
}
int Core::ViewportController::GetFixedTop() const
{
    return mFixedContext.mTop;
}
int Core::ViewportController::GetFixedWidth() const
{
    return mFixedContext.mWidth;
}
int Core::ViewportController::GetFixedHeight() const
{
    return mFixedContext.mHeight;
}

bool Core::ViewportController::GetAnchorLeftState() const
{
    return mAnchoredContext.mLeftAnchor.mAnchorOn;
}
bool Core::ViewportController::GetAnchorRightState() const
{
    return mAnchoredContext.mRightAnchor.mAnchorOn;
}
bool Core::ViewportController::GetAnchorTopState() const
{
    return mAnchoredContext.mTopAnchor.mAnchorOn;
}
bool Core::ViewportController::GetAnchorBottomState() const
{
    return mAnchoredContext.mBottomAnchor.mAnchorOn;
}

Core::EViewportAnchoredMode Core::ViewportController::GetAnchorLeftMode() const
{
    return mAnchoredContext.mLeftAnchor.mAnchorMode;
}
Core::EViewportAnchoredMode Core::ViewportController::GetAnchorRightMode() const
{
    return mAnchoredContext.mRightAnchor.mAnchorMode;
}
Core::EViewportAnchoredMode Core::ViewportController::GetAnchorTopMode() const
{
    return mAnchoredContext.mTopAnchor.mAnchorMode;
}
Core::EViewportAnchoredMode Core::ViewportController::GetAnchorBottomMode() const
{
    return mAnchoredContext.mBottomAnchor.mAnchorMode;
}

float Core::ViewportController::GetAnchorLeftRelValue() const
{
    return mAnchoredContext.mLeftAnchor.mRel;
}
float Core::ViewportController::GetAnchorRightRelValue() const
{
    return mAnchoredContext.mRightAnchor.mRel;
}
float Core::ViewportController::GetAnchorTopRelValue() const
{
    return mAnchoredContext.mTopAnchor.mRel;
}
float Core::ViewportController::GetAnchorBottomRelValue() const
{
    return mAnchoredContext.mBottomAnchor.mRel;
}

uint32_t Core::ViewportController::GetAnchorLeftPixelValue() const
{
    return mAnchoredContext.mLeftAnchor.mPixel;
}
uint32_t Core::ViewportController::GetAnchorRightPixelValue() const
{
    return mAnchoredContext.mRightAnchor.mPixel;
}
uint32_t Core::ViewportController::GetAnchorTopPixelValue() const
{
    return mAnchoredContext.mTopAnchor.mPixel;
}
uint32_t Core::ViewportController::GetAnchorBottomPixelValue() const
{
    return mAnchoredContext.mBottomAnchor.mPixel;
}

Core::EViewportAnchoredMode Core::ViewportController::GetAnchorWidthSizeMode() const
{
    return mAnchoredContext.mWidth.mAnchorMode;
}
Core::EViewportAnchoredMode Core::ViewportController::GetAnchorHeightSizeMode() const
{
    return mAnchoredContext.mHeight.mAnchorMode;
}

float Core::ViewportController::GetAnchorWidthRelValue() const
{
    return mAnchoredContext.mWidth.mRel;
}
float Core::ViewportController::GetAnchorHeightRelValue() const
{
    return mAnchoredContext.mHeight.mRel;
}

uint32_t Core::ViewportController::GetAnchorWidthPixelValue() const
{
    return mAnchoredContext.mWidth.mPixel;
}
uint32_t Core::ViewportController::GetAnchorHeightPixelValue() const
{
    return mAnchoredContext.mHeight.mPixel;
}