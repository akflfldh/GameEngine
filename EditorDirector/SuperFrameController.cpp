#include "EditorDirector/SuperFrameController.h"
#include <Core/GlobalAppHelper.h>
#include <Core/LogicalWindow.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/Texture.h>
// #include <CoreAsset/TextureManager.h>
#include <Core/Application.h>
#include <Core/CameraComponent.h>
#include <Core/CameraObject.h>
#include <Core/Entity.h>
#include <Core/Map.h>
#include <Core/StaticMeshComponent.h>
#include <Core/StaticMeshObject.h>
#include <CoreAsset/StaticMesh.h>
#include <CoreDevice/D3DCoreDevice.h>
#include <EditorDirector/EditorAssetManager.h>
#include <EditorDirector/EditorConfig.h>
#include <EditorDirector/EditorSceneController.h>
#include <EditorDirector/ImGuiSystem.h>
#include <EditorDirector/UIAssetBrowser.h>
#include <EditorDirector/UIScrollBox.h>
#include <GlobalOverlayManager.h>
#include <InputSystem/InputSystem.h>
#include <Logger/Logger.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <ObjectHierarchyPanel.h>
#include <RenderFrontend/RenderPipelineManager.h>
#include <RenderFrontend/UIRenderItemBuilder.h>
#include <RenderSystem/IRenderSystem.h>
#include <SystemInitializer/ISystemInitializer.h>
#include <UISystem/UIElement.h>
#include <UISystem/UIImageComponent.h>
#include <UISystem/UIManager.h>
#include <UISystem/UIResizeGizmoRenderableComponent.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIMovableComponent.h>
#include <UiSystem/UITextComponent.h>
#include <Utility/Utility.h>
#include <Window/BaseWindow.h>
#include <sstream>

#ifdef D3DX
#include "EditorDirector/ImGui/imgui.h"
#include <SystemInitializer/D3DSystemInitializer.h>
#include <windowsx.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

Quad::SuperFrameController::~SuperFrameController() {}

Quad::SuperFrameController *Quad::SuperFrameController::GetInstance()
{
    static SuperFrameController controller;

    return &controller;
}

void Quad::SuperFrameController::Initialize(Render::RenderPipelineManager &renderPipelineManager)
{

#ifdef D3DX

    Core::D3DCoreDevice *d3dCoreDevice = static_cast<Core::D3DCoreDevice *>(Core::CoreDevice::GetInstance());
    ImGuiSystem *imGuiSystem = ImGuiSystem::GetInstance();

    imGuiSystem->Initialize();

#endif

    mWindow = new BaseWindow(Core::GlobalAppHelper::GetHinstance());

    mWindow->Initialize();
    mWindow->SetIWindowEventHandler(this);

    if (EditorConfig::GetInstance()->GetEditorMode() == EEditorMode::eProjectBrowser)
    {
        mWindow->SetMaxClientWidth(3000);
        mWindow->SetMaxClientHeight(3000);
        mWindow->SetClientWidth(1000);
        mWindow->SetClientHeight(1150);
        mWindow->CreateWindowClass(L"FrameWindow", L"Editor", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
    }
    else
    {

        mWindow->CreateWindowClass(L"FrameWindow", L"Editor");
    }

    InputSystem *inputSystem = InputSystem::GetInstance();
    Render::IRenderSystem *renderSystem = Render::IRenderSystem::GetInstance();

    RECT windowClientRect2;
    GetClientRect(mWindow->GetWindowHandle(), &windowClientRect2);

#ifdef D3DX

    D3DSystemInitializer::D3DSystemInitializer *d3dSystemInitializer =
        static_cast<D3DSystemInitializer::D3DSystemInitializer *>(SystemInitializer::ISystemInitializer::GetInstance());
    imGuiSystem->InitPlatform(mWindow->GetWindowHandle(), d3dCoreDevice->mDevice.Get(),
                              d3dCoreDevice->mCommandQueue.Get());

    imGuiSystem->SetDependency(QuadLF::LogicalFileSystem::GetInstance(), nullptr, nullptr,
                               EditorAssetManager::GetInstance());

#endif

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

    mWindow->SetMaxClientWidth(3000);
    mWindow->SetMaxClientHeight(3000);

    InitWorld();
}

void Quad::SuperFrameController::SetWorkSpace(Core::WorkSpace *workspace)
{
    SuperController::SetWorkSpace(workspace);

    workspace->OnResizeWindow(mWindow->GetClientWidth(), mWindow->GetClientHeight());

    for (int i = 0; i < workspace->mWindowList.size(); ++i)
    {
        workspace->mWindowList[i]->SetOwnerController(this);
    }
}

// void Quad::SuperFrameController::AddLogicalWindow(Core::LogicalWindow *window)
//{
//
//     //mLogicalWindowList.push_back(window);
//   //  window->SetOwnerController(this);
// }

void Quad::SuperFrameController::SetGlobalOverlayWindow(Core::LogicalWindow *window)
{
    //  mGlobalOverlayLogicalWindow = window;
}

void Quad::SuperFrameController::OnInput()
{

    InputSystem *inputSystem = InputSystem::GetInstance();

    if (inputSystem)
    {
        inputSystem->SetCurrentWindowHandle(mWindow->GetWindowHandle());
    }
}

void Quad::SuperFrameController::SetMousePos(int sx, int sy, int cx, int cy)
{

    InputSystem *inputSystem = InputSystem::GetInstance();

    if (inputSystem)
    {
        inputSystem->SetMousePos(sx, sy, cx, cy);
    }
}

void Quad::SuperFrameController::OnMouseMove(int deltaX, int deltaY)
{

    InputSystem *inputSystem = InputSystem::GetInstance();

    if (inputSystem)
    {
        inputSystem->OnMouseMove(deltaX, deltaY);
    }
}

void Quad::SuperFrameController::OnMouseWheel(int wheelDelta)
{

    InputSystem *inputSystem = InputSystem::GetInstance();

    if (inputSystem)
    {
        inputSystem->OnMouseWheel(wheelDelta);
    }
}

void Quad::SuperFrameController::OnWindowMinimized()
{

    mRenderStop = true;
    Quad::Application::GetInstance()->SetMinimizeFlag(true);
}

void Quad::SuperFrameController::OnWindowRestored()
{

    mRenderStop = false;
}

void Quad::SuperFrameController::OnWindowResize(unsigned int width, unsigned int height)
{
    if (mRenderStop)
    {
        mRenderStop = false;
        Quad::Application::GetInstance()->SetMinimizeFlag(false);
    }
    else
    {

        OnResizeWindow(width, height);
    }
}

void Quad::SuperFrameController::OnMouseButtonEvent(EInputState mouseButtonState)
{

    // Convert WindowMouseState - > InputSystemMouseState

    InputSystem *inputSystem = InputSystem::GetInstance();
    if (inputSystem)
    {
        inputSystem->OnMouseButtonEvent(mouseButtonState);
    }
}

void Quad::SuperFrameController::OnKeyEvent(EInputState keyState, uint8_t scanKey)
{

    InputSystem *inputSystem = InputSystem::GetInstance();
    if (inputSystem)
    {
        inputSystem->OnKeyEvent(keyState, scanKey);
    }
}

void Quad::SuperFrameController::OnCharEvent(uint32_t ch)
{
    InputSystem *inputSystem = InputSystem::GetInstance();
    if (inputSystem)
    {
        inputSystem->OnCharEvent(ch);
    }
}

void Quad::SuperFrameController::InitWorld()
{
    SuperController::InitWorld();
    // map생성 world 등록
    InitMap();
    TestMap();
}

void Quad::SuperFrameController::Begin()
{

    /*   for (auto logicalWindow : mLogicalWindowList)
       {
           logicalWindow->Begin();
       }*/
}

void Quad::SuperFrameController::PreUpdate()
{

    if (mRenderStop)
    {

        int a = 2;
    }
    else
    {
        mBackBufferIndex = (mBackBufferIndex + 1) % 2;
    }
}

void Quad::SuperFrameController::Update(float deltaTime)
{

    InputSystem *inputSystem = InputSystem::GetInstance();
    if (inputSystem)
    {

        if (mWindow && mWindow->GetWindowHandle() == inputSystem->GetCurrentWindowHandle())
        {
            auto handler = std::bind(&SuperFrameController::OnInputEvent, this, std::placeholders::_1);
            inputSystem->Dispatch(handler);
        }
    }
}

void Quad::SuperFrameController::EndUpdate() {}

void Quad::SuperFrameController::Draw(Render::RenderPipelineManager &renderPipelineManager)
{

    if (mRenderStop)
        return;

    Render::IRenderSystem *renderSystem = Render::IRenderSystem::GetInstance();

    if (mWorkSpace == nullptr)
        return;

    mWorkSpace->UpdateFrameData();

    // for (size_t i = 0; i < mLogicalWindowList.size(); ++i)
    //{
    //     mLogicalWindowList[i]->UpdateFrameData();
    // }

    // LogicalWindow에서 약간은 RenderPass들의 사용을 제어할수잇는 옵션이 필요하다.

    auto app = Quad::Application::GetInstance();
    renderPipelineManager.Execute(mWorkSpace->mWindowList, mWindow->GetWindowHandle(), app->GetCurrentFrameIndex(),
                                  app->GetCurrentFrameFenceValue(), true, mBackBufferIndex,
                                  Application::GetInstance()->GetTotalFrameCount());
}

void Quad::SuperFrameController::ShutDownWindow()
{

    if (mWindow)
    {
        mWindow->ShutDown();
    }
}

Quad::BaseWindow *Quad::SuperFrameController::GetWindow() const
{
    return mWindow;
}

void Quad::SuperFrameController::TestLButtonDownCallback()
{

    MessageBoxW(mWindow->GetWindowHandle(), L"마우스 클릭!", L"알림", MB_OK);
}

void Quad::SuperFrameController::OnInputEvent(const Quad::RawInputData &inputData)
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

void Quad::SuperFrameController::SetMouseCapture(Core::LogicalWindow *window)
{
    if (window == nullptr)
        return;
    mMouseCapturedLogicalWindow = window;
    GetWindow()->SetMouseCapture(true);
}

void Quad::SuperFrameController::ReleaseMouseCapture()
{
    mMouseCapturedLogicalWindow = nullptr;
    GetWindow()->SetMouseCapture(false);
}

void Quad::SuperFrameController::SetKeyboardCapture(Core::LogicalWindow *window)
{
    if (window == nullptr)
        return;
    mKeyboardCapturedLogicalWindow = window;
    GetWindow()->SetKeyboardCapture(true);
}

void Quad::SuperFrameController::ReleaseKeyboardCapture()
{

    mKeyboardCapturedLogicalWindow = nullptr;
    GetWindow()->SetKeyboardCapture(false);
}

std::pair<uint32_t, uint32_t> Quad::SuperFrameController::GetWindowSize() const
{
    return {GetWindow()->GetClientWidth(), GetWindow()->GetClientHeight()};
}

void Quad::SuperFrameController::OnResizeWindow(UINT clientWidth, UINT clientHeight)
{
    // Render::IRenderSystem *renderSystem = Render::IRenderSystem::GetInstance();

    mBackBufferIndex = Render::RenderPipelineManager::GetInstance()->WindowResize(mWindow->GetWindowHandle());
    mBackBufferIndex -= 1; // 그래야 다음 PreUpdate에서 다시 원래 0,1값으로시작

    mWindow->SetClientWidth(clientWidth);
    mWindow->SetClientHeight(clientHeight);

    // if (renderSystem != nullptr)
    //{
    //     renderSystem->WindowResize(mWindow->GetWindowHandle());
    // }

    if (mUICanvas != nullptr)
    {
        mUICanvas->SetSize({(float)clientWidth, (float)clientHeight});
    }

    if (mWorkSpace)
        mWorkSpace->OnResizeWindow(clientWidth, clientHeight);
    /*  for (auto logicalWindow : mLogicalWindowList)
      {
          logicalWindow->OnResizeWindow(clientWidth, clientHeight);
      }*/
    // 각 채널 viewport 도 조정
    //  ResizeViewportControllers(clientWidth, clientHeight);
}

void Quad::SuperFrameController::InitMap() {}

void Quad::SuperFrameController::TestMap() {}

Core::LogicalWindow *Quad::SuperFrameController::GetLogicalWindowOnMouse(const Quad::RawInputData &inputData)
{

    InputSystem *inputSystem = InputSystem::GetInstance();

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
                GlobalOverlayManager::GetInstance()->CloseCurrentContextMenuAll();
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

Quad::SuperFrameController::SuperFrameController() : mRenderStop(false) {}
