#include "WindowedFrameController.h"
#include <Core/Application.h>
#include <Core/GlobalAppHelper.h>
#include <Core/LogicalWindow.h>
#include <Core/WorkSpace.h>
#include <CoreBase/CallbackSystem.h>
#include <InputSystem/InputSystem.h>
#include <RenderSystem/IRenderSystem.h>
#include <UiSystem/UICanvas.h>
#include <Utility/Utility.h>
#include <Window/BaseWindow.h>

Core::WindowedFrameController::WindowedFrameController() {}

Core::WindowedFrameController::~WindowedFrameController() {}

void Core::WindowedFrameController::Begin() {}

void Core::WindowedFrameController::PreUpdate() {}

void Core::WindowedFrameController::Update(float deltaTime)
{

    Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance();
    if (inputSystem)
    {

        if (mWindow && mWindow->GetWindowHandle() == inputSystem->GetCurrentWindowHandle())
        {
            auto handler = std::bind(&WindowedFrameController::OnInputEvent, this, std::placeholders::_1);
            inputSystem->Dispatch(handler);
        }
    }
}

void Core::WindowedFrameController::EndUpdate() {}
void Core::WindowedFrameController::Draw()
{

    if (mRenderStop)
        return;

    Render::IRenderSystem *renderSystem = Render::IRenderSystem::GetInstance();

    if (mWorkSpace == nullptr)
        return;

    mWorkSpace->UpdateFrameData();

    DrawWindow();
}
void Core::WindowedFrameController::ShutDownWindow()
{

    if (mWindow)
    {
        mWindow->ShutDown();
    }
}

void Core::WindowedFrameController::SetWorkSpace(Core::WorkSpace *workspace)
{
    mWorkSpace = workspace;
    workspace->OnResizeWindow(mWindow->GetClientWidth(), mWindow->GetClientHeight());

    for (int i = 0; i < workspace->mWindowList.size(); ++i)
    {
        workspace->mWindowList[i]->SetOwnerController(this);
    }
}

void Core::WindowedFrameController::OnInput()
{

    Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance();

    if (inputSystem)
    {
        inputSystem->SetCurrentWindowHandle(mWindow->GetWindowHandle());
    }
}

void Core::WindowedFrameController::SetMousePos(int sx, int sy, int cx, int cy)
{

    Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance();

    if (inputSystem)
    {
        inputSystem->SetMousePos(sx, sy, cx, cy);
    }
}

void Core::WindowedFrameController::OnMouseMove(int deltaX, int deltaY)
{

    Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance();

    if (inputSystem)
    {
        inputSystem->OnMouseMove(deltaX, deltaY);
    }
}

void Core::WindowedFrameController::OnMouseWheel(int wheelDelta)
{

    Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance();

    if (inputSystem)
    {
        inputSystem->OnMouseWheel(wheelDelta);
    }
}

void Core::WindowedFrameController::OnMouseButtonEvent(EInputState mouseButtonState)
{

    // Convert WindowMouseState - > InputSystemMouseState

    Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance();
    if (inputSystem)
    {
        inputSystem->OnMouseButtonEvent(mouseButtonState);
    }
}

void Core::WindowedFrameController::OnKeyEvent(EInputState keyState, uint8_t scanKey)
{

    Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance();
    if (inputSystem)
    {
        inputSystem->OnKeyEvent(keyState, scanKey);
    }
}

void Core::WindowedFrameController::OnCharEvent(uint32_t ch)
{
    Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance();
    if (inputSystem)
    {
        inputSystem->OnCharEvent(ch);
    }
}

void Core::WindowedFrameController::OnWindowMinimized()
{
    mRenderStop = true;
    Quad::Application::GetInstance()->SetMinimizeFlag(true);
}

void Core::WindowedFrameController::OnWindowRestored()
{
    mRenderStop = false;
}

void Core::WindowedFrameController::OnWindowResize(unsigned int width, unsigned int height)
{

    if (mRenderStop)
    {
        mRenderStop = false;
        Quad::Application::GetInstance()->SetMinimizeFlag(false);
    }
    else
    {
        mWindow->SetClientWidth(width);
        mWindow->SetClientHeight(height);

        // if (renderSystem != nullptr)
        //{
        //     renderSystem->WindowResize(mWindow->GetWindowHandle());
        // }

        if (mWorkSpace)
            mWorkSpace->OnResizeWindow(width, height);
    }

    OnRenderSurfaceResize(width, height);
}

void Core::WindowedFrameController::OnInputEvent(const Quad::RawInputData &inputData)
{

    Core::LogicalWindow *window = nullptr;

    if (inputData.IsMouseEvent())
    {

        if (mMouseCapturedLogicalWindow)
        {
            window = mMouseCapturedLogicalWindow;
        }
    }
    else if (inputData.IsKeyboardEvent())
    {
        // 캡처한키보드가있더라,

        if (mKeyboardCapturedLogicalWindow)
        {
            window = mKeyboardCapturedLogicalWindow;
        }
    }

    if (window == nullptr)
        window = GetLogicalWindowOnMouse(inputData);

    if (window)
    {
        Core::GlobalAppHelper *globalAppHelper = Core::GlobalAppHelper::GetInstance();

        if (inputData.IsMouseEvent())
        {
            globalAppHelper->SetCurrentMouseActiveLogicalWindow(window);
            if (inputData.mInputState & EInputState::eMouseLButtonDown)
            {
                globalAppHelper->SetCurrentKeyboardActiveLogicalWindow(window);
                mKeyboardCapturedLogicalWindow = nullptr;
            }
        }
        /*      else if (inputData.IsKeyboardEvent())
              {
                  globalAppHelper->SetCurrentKeyboardActiveLogicalWindow(window);
              }*/

        window->OnInputEvent(inputData);
    }
}

Quad::BaseWindow *Core::WindowedFrameController::GetWindow() const
{

    return mWindow.get();
}
// World *Core::WindowedFrameController::GetWorld() const
//{
//
//     return mWorld.get();
// }

void Core::WindowedFrameController::SetMouseCapture(Core::LogicalWindow *window)
{
    if (window == nullptr)
        return;
    mMouseCapturedLogicalWindow = window;
    GetWindow()->SetMouseCapture(true);
}

void Core::WindowedFrameController::ReleaseMouseCapture()
{
    mMouseCapturedLogicalWindow = nullptr;
    GetWindow()->SetMouseCapture(false);
}

void Core::WindowedFrameController::SetKeyboardCapture(Core::LogicalWindow *window)
{
    if (window == nullptr)
        return;
    mKeyboardCapturedLogicalWindow = window;
    GetWindow()->SetKeyboardCapture(true);
}

void Core::WindowedFrameController::ReleaseKeyboardCapture()
{

    mKeyboardCapturedLogicalWindow = nullptr;
    GetWindow()->SetKeyboardCapture(false);
}

std::pair<uint32_t, uint32_t> Core::WindowedFrameController::GetWindowSize() const
{
    return {GetWindow()->GetClientWidth(), GetWindow()->GetClientHeight()};
}

Core::WorkSpace *Core::WindowedFrameController::GetWorkSpace() const
{

    return mWorkSpace;
}

// void Core::WindowedFrameController::SetWorkSpace(Core::WorkSpace *workspace)
//{
//
//     mWorkSpace = workspace;
// }

void Core::WindowedFrameController::ShowWindow()
{

    if (mWindow)
    {
        mWindow->Show();
    }
}

void Core::WindowedFrameController::MaximizedWindow()
{

    if (mWindow)
    {

        mWindow->Show(SW_SHOWMAXIMIZED);
    }
}

void Core::WindowedFrameController::InitializeWindow(const WindowCreateDesc &creationDesc)
{

#ifdef D3DX

#endif

    mWindow = std::make_unique<Quad::BaseWindow>((Core::GlobalAppHelper::GetHinstance()));

    mWindow->Initialize();
    mWindow->SetIWindowEventHandler(this);

    mWindow->SetMaxClientWidth(creationDesc.mMaxClientWidth);
    mWindow->SetMaxClientHeight(creationDesc.mMaxClientHeight);
    mWindow->SetClientWidth(creationDesc.mClientWidth);
    mWindow->SetClientHeight(creationDesc.mClientHeight);
    mWindow->CreateWindowClass(CoreUtility::Utility::ConvertToWString(creationDesc.mWindowClassName, true).c_str(),
                               CoreUtility::Utility::ConvertToWString(creationDesc.mWindowName, true).c_str(),
                               creationDesc.mWindowStyle, creationDesc.mWindowClassStyle);

    // if (EditorConfig::GetInstance()->GetEditorMode() == EEditorMode::eProjectBrowser)
    //{
    //     mWindow->SetMaxClientWidth(3000);
    //     mWindow->SetMaxClientHeight(3000);
    //     mWindow->SetClientWidth(1000);
    //     mWindow->SetClientHeight(1150);
    //     mWindow->CreateWindowClass(L"FrameWindow", L"Editor", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
    //     WS_MINIMIZEBOX); mWindow->Show();
    // }
    // else
    //{
    //     // 최대크기

    //    mWindow->CreateWindowClass(L"FrameWindow", L"Editor");
    //    mWindow->Show(SW_SHOWMAXIMIZED);

    //    RECT windowClientRect2;
    //    GetClientRect(mWindow->GetWindowHandle(), &windowClientRect2);

    //    mWindow->SetMaxClientWidth(windowClientRect2.right);
    //    mWindow->SetMaxClientHeight(windowClientRect2.bottom);
    //    mWindow->SetClientWidth(windowClientRect2.right);
    //    mWindow->SetClientHeight(windowClientRect2.bottom);
    //}

    // InputSystem *inputSystem = InputSystem::GetInstance();
    Render::IRenderSystem *renderSystem = Render::IRenderSystem::GetInstance();

    Render::CreationRenderChannelInfo creationRenderChannelInfo;
    creationRenderChannelInfo.mWindowHandle = mWindow->GetWindowHandle();
    RECT windowClientRect;
    GetClientRect((HWND)creationRenderChannelInfo.mWindowHandle, &windowClientRect);

    creationRenderChannelInfo.mWidth = windowClientRect.right;
    creationRenderChannelInfo.mHeight = windowClientRect.bottom;
    creationRenderChannelInfo.mLeft = 0;
    creationRenderChannelInfo.mTop = 0;
    creationRenderChannelInfo.mMinZ = 0;
    creationRenderChannelInfo.mMaxZ = 1.0f;

    renderSystem->RegisterWindow(creationRenderChannelInfo);

    // InitWorld();
}

void Core::WindowedFrameController::OnRenderSurfaceResize(uint32_t width, uint32_t height) {}

Core::LogicalWindow *Core::WindowedFrameController::GetLogicalWindowOnMouse(const Quad::RawInputData &inputData)
{

    Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance();

    Quad::MouseContext mouseContext = inputSystem->GetMouseContext();

    if (mWorkSpace == nullptr)
        return nullptr;

    Core::LogicalWindow *window = nullptr;

    // 전역오버레이 window 먼저 ui와 충돌하는지 판정
    if (mWorkSpace->mGloberOverlayWindow)
    {

        if (mWorkSpace->mGloberOverlayWindow->HitTestUI(mouseContext.mClientPosX, mouseContext.mClientPosY))
        {
            window = mWorkSpace->mGloberOverlayWindow;

            // return mWorkSpace->mGloberOverlayWindow;
        }
        else
        {
            if (inputData.IsMouseEvent() && inputData.mInputState & EInputState::eMouseLButtonDown)
            {
                OnMouseDownOutsideOverlay();
            }
        }
    }

    if (window == nullptr)
    {

        for (auto logicalWindow : mWorkSpace->mWindowList)
        {
            if (logicalWindow->mViewportController.IntersectPoint(mouseContext.mClientPosX, mouseContext.mClientPosY))
            {
                window = logicalWindow;
                break;
                //  return logicalWindow;
            }
        }
    }

    if (mCurrentLogicalWindowOnMouse && (window != mCurrentLogicalWindowOnMouse))
    {
        mCurrentLogicalWindowOnMouse->OnMouseLeave();
    }

    if (window && (window != mCurrentLogicalWindowOnMouse))
    {
        //
        mOnMouseEnterWindowCallbackSystem.ExecuteCallbacks(window);
        window->OnMouseEnter();
    }

    mCurrentLogicalWindowOnMouse = window;

    return window;
}

void Core::WindowedFrameController::OnMouseDownOutsideOverlay() {}

void Core::WindowedFrameController::DrawWindow() {}
