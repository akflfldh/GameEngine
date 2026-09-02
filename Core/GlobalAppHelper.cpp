#include "Core/GlobalAppHelper.h"
#include <Core/LogicalWindow.h>
#include <Core/SuperController.h>
HINSTANCE Core::GlobalAppHelper::mHinstance;

void Core::GlobalAppHelper::Initialize(HINSTANCE hInstance)
{
    mHinstance = hInstance;
}

Core::GlobalAppHelper *Core::GlobalAppHelper::GetInstance()
{
    static GlobalAppHelper helper;

    return &helper;
}

HINSTANCE Core::GlobalAppHelper::GetHinstance()
{
    return mHinstance;
}

void Core::GlobalAppHelper::SetCurrentMouseActiveLogicalWindow(Core::LogicalWindow *window)
{

    mCurrMouseActiveLogicalWindow = window;
}

void Core::GlobalAppHelper::SetCurrentKeyboardActiveLogicalWindow(Core::LogicalWindow *window)
{

    if (mCurrKeyboardActiveLogicalWindow != window)
    {
        // 기존의 mCurrKeyboardActiveLogicalWindow 쪽에게 keyboard캡처가 풀린다라고 전달
        mCurrKeyboardActiveLogicalWindow->OnNotifyReleasedKeyboardCapture();
    }

    mCurrKeyboardActiveLogicalWindow = window;
}

void Core::GlobalAppHelper::NotifyMouseCapture()
{
    if (mCurrMouseActiveLogicalWindow)
    {
        Core::SuperController *superController = mCurrMouseActiveLogicalWindow->GetOwnerController();

        superController->SetMouseCapture(mCurrMouseActiveLogicalWindow);
    }
}

void Core::GlobalAppHelper::NotifyMouseReleaseCapture()
{

    if (mCurrMouseActiveLogicalWindow)
    {
        Core::SuperController *superController = mCurrMouseActiveLogicalWindow->GetOwnerController();

        superController->ReleaseMouseCapture();
    }
}

void Core::GlobalAppHelper::NotifyKeyboardCapture()
{

    if (mCurrKeyboardActiveLogicalWindow)
    {
        Core::SuperController *superController = mCurrKeyboardActiveLogicalWindow->GetOwnerController();
        superController->SetKeyboardCapture(mCurrKeyboardActiveLogicalWindow);
    }
}

void Core::GlobalAppHelper::NotifyKeyboardReleaseCapture()
{

    if (mCurrKeyboardActiveLogicalWindow)
    {
        Core::SuperController *superController = mCurrKeyboardActiveLogicalWindow->GetOwnerController();
        superController->ReleaseKeyboardCapture();
    }
}

Core::LogicalWindow *Core::GlobalAppHelper::GetCurrMouseActiveLogicalWindow() const
{
    return mCurrMouseActiveLogicalWindow;
}
