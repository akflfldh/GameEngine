#include "UIButtonComponent.h"

UI::UIButtonComponent::UIButtonComponent() : mCaptureAvailableFlag(true) {}

UI::UIButtonComponent::~UIButtonComponent() {}

void UI::UIButtonComponent::Update() {}

bool UI::UIButtonComponent::IsPointInside(float x, float y) const
{
    return IsPointInsideDefault(x, y);
}

void UI::UIButtonComponent::UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                                  bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{

    if (mouseInputContext.mCaptureState != EUIMouseCaptureType::eNone)
    {
        UpdateOnCapture(mouseInputContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
    }
    else
    {
        UpdateOnHover(mouseInputContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
    }
}

void UI::UIButtonComponent::SetCaptureAvailable(bool flag)
{
    mCaptureAvailableFlag = flag;
}

// 캡처상태가아닌 hover상태에서의 업데이트
void UI::UIButtonComponent::UpdateOnHover(const UIManagerMouseInputContext &mouseInputContext,
                                          bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{
    // hover enter

    // hover held
    //  lbutton click
    // if capture avaliable flag    == true -> capture : false -> click상태만유지

    // lbutton release
    // click상태라면 -> onclick, 클릭상태 해제

    // hover release
    // clikc상태라면 ->  클릭상태 해제

    switch (mouseInputContext.mHoverState)
    {
    case EUIMouseHoverType::eEnter:
        // hover enter되었을떄 호출할 메서드를 둘 수도있다.

        break;
    case EUIMouseHoverType::eHeld:

        if (mouseInputContext.mMouseContext.mMouseState & (Quad::EMouseState::eLButtonPressed))
        {
            mPressedState = true;
            if (mCaptureAvailableFlag == true)
            {
                oCaptureActiveRequestFlag = true;
            }
        }

        if (mouseInputContext.mMouseContext.mMouseState & (Quad::EMouseState::eLButtonReleased))
        {
            if (mPressedState == true)
            {

                // 클릭한것이다.
                OnClick();
                mPressedState = false;
            }
        }

        break;
    case EUIMouseHoverType::eRelease:

        if (mouseInputContext.mMouseContext.mMouseState & (Quad::EMouseState::eLButtonReleased))
        {
            if (mPressedState == true)
            {
                // 클릭실패
                mPressedState = false;
            }
        }

        break;
    }
}

// 캡처상태일떄의 업데이트
void UI::UIButtonComponent::UpdateOnCapture(const UIManagerMouseInputContext &mouseInputContext,
                                            bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{
    // 눌럿을때만 캡처된다.
    switch (mouseInputContext.mCaptureState)
    {
    case EUIMouseCaptureType::eEnter:

        break;

    case EUIMouseCaptureType::eHeld:

        if (mouseInputContext.mMouseContext.mMouseState & (Quad::EMouseState::eLButtonReleased))
        {
            if (mPressedState == true)
            {

                // 클릭한것이다.
                OnClick();
                mPressedState = false;
            }
            oCaptureReleaseRequestFlag = true;
        }

        break;

    case EUIMouseCaptureType::eRelease:
        if (mouseInputContext.mMouseContext.mMouseState & (Quad::EMouseState::eLButtonReleased))
        {

            mPressedState = false;
            oCaptureReleaseRequestFlag = true;
        }

        break;
    }
}
