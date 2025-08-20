#include "EditorDirector/SuperFrameController.h"
#include <Core/GlobalAppHelper.h>
#include <Core/LogicalWindow.h>
#include <Core/UIRenderItemBuilder.h>
#include <CoreAsset/MaterialManager.h>
#include <CoreAsset/TextureManager.h>
#include <CoreDevice/D3DCoreDevice.h>
#include <EditorDirector/ImGuiSystem.h>
#include <InputSystem/InputSystem.h>
#include <Logger/Logger.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <RenderSystem/IRenderSystem.h>
#include <SystemInitializer/ISystemInitializer.h>
#include <UISystem/UIElement.h>
#include <UISystem/UIImageComponent.h>
#include <UISystem/UIManager.h>
#include <UISystem/UIResizeGizmoRenderableComponent.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIMovableComponent.h>
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

void Quad::SuperFrameController::Initialize()
{

#ifdef D3DX

    Core::D3DCoreDevice *d3dCoreDevice = static_cast<Core::D3DCoreDevice *>(Core::CoreDevice::GetInstance());
    ImGuiSystem *imGuiSystem = ImGuiSystem::GetInstance();

    imGuiSystem->Initialize();

#endif

    mWindow = new BaseWindow(GlobalAppHelper::GetHinstance());

    mWindow->Initialize(std::bind(&SuperFrameController::WndProc, this, std::placeholders::_1, std::placeholders::_2,
                                  std::placeholders::_3, std::placeholders::_4));
    mWindow->CreateWindowClass(L"FrameWindow", L"FrameWindow");

    InputSystem *inputSystem = InputSystem::GetInstance();

    Render::IRenderSystem *renderSystem = Render::IRenderSystem::GetInstance();

    RECT windowClientRect2;
    GetClientRect(mWindow->GetWindowHandle(), &windowClientRect2);

#ifdef D3DX

    D3DSystemInitializer::D3DSystemInitializer *d3dSystemInitializer =
        static_cast<D3DSystemInitializer::D3DSystemInitializer *>(SystemInitializer::ISystemInitializer::GetInstance());
    imGuiSystem->InitPlatform(mWindow->GetWindowHandle(), d3dCoreDevice->mDevice.Get(),
                              d3dCoreDevice->mCommandQueue.Get());

    imGuiSystem->SetDependency(QuadLF::LogicalFileSystem::GetInstance(), CoreAsset::TextureManager::GetInstance(),
                               CoreAsset::MaterialManager::GetInstance());

#endif

    //  mUISystem = UI::UISystem::GetInstance();

    // RenderChannel
    Render::CreationRenderChannelInfo creationRenderChannelInfo;
    creationRenderChannelInfo.mWindowHandle = mWindow->GetWindowHandle();
    RECT windowClientRect;
    GetClientRect(creationRenderChannelInfo.mWindowHandle, &windowClientRect);

    creationRenderChannelInfo.mWidth = windowClientRect.right;
    creationRenderChannelInfo.mHeight = windowClientRect.bottom;
    creationRenderChannelInfo.mLeft = 0;
    creationRenderChannelInfo.mTop = 0;
    creationRenderChannelInfo.mMinZ = 0;
    creationRenderChannelInfo.mMaxZ = 1.0f;

    mImGuiLogicalWindow.SetRenderChannelID(renderSystem->RegisterRenderChannel(creationRenderChannelInfo));
    // mImGuiRenderChannelID = renderSystem->RegisterRenderChannel(creationRenderChannelInfo);
    mSubLogicalWindow.SetRenderChannelID(renderSystem->RegisterRenderChannel(creationRenderChannelInfo));
    // mSubRenderChannelID = renderSystem->RegisterRenderChannel(creationRenderChannelInfo);

    // canvas

    CoreAsset::MaterialManager *materialManager = CoreAsset::MaterialManager::GetInstance();

    UI::UIManager *uiManager = UI::UIManager::GetInstance();

    // Canvas1 (default)
    UI::UICanvasID canvasID = uiManager->CreateCanvas("DefaultCanvas", UI::ECanvasSizeMode::eFixSize);

    mUICanvas = uiManager->GetCanvas(canvasID);
    UI::UIElement *defaultUIElement = mUICanvas->CreateUIElement<UI::UIElement>("DefaultUIElement");

    defaultUIElement->mTransform.SetSize({100, 100});
    defaultUIElement->mTransform.SetPositionLocal({200, 200});

    UI::UIImageComponent *defaultRenderCom =
        defaultUIElement->CreateUIComponent<UI::UIImageComponent>("DefaultRenderCom");

    defaultRenderCom->SetTexture(CoreAsset::TextureManager::GetInstance()->GetAsset(1));
    defaultRenderCom->SetTemplateMaterial(1);

    UI::UIElement *defaultUIElement2 = mUICanvas->CreateUIElement<UI::UIElement>("DefaultUIElement2");

    defaultRenderCom = defaultUIElement2->CreateUIComponent<UI::UIImageComponent>("DefaultRenderCom");

    defaultUIElement2->mTransform.SetSize({400, 400});
    defaultUIElement2->mTransform.SetPositionLocal({400, 400});
    defaultRenderCom->SetTexture(CoreAsset::TextureManager::GetInstance()->GetAsset(1));
    defaultRenderCom->SetTemplateMaterial(1);

    // Canvas2   다른채널
    {
        UI::UICanvasID canvas2ID = uiManager->CreateCanvas("DefaultCanvas2", UI::ECanvasSizeMode::eFixSize);
        mUICanvas2 = uiManager->GetCanvas(canvas2ID);

        UI::UIElement *defaultUIElement = mUICanvas2->CreateUIElement<UI::UIElement>("DefaultUIElement3");

        defaultUIElement->mTransform.SetSize({100, 100});
        defaultUIElement->mTransform.SetPositionLocal({0, 0});

        UI::UIImageComponent *defaultRenderCom =
            defaultUIElement->CreateUIComponent<UI::UIImageComponent>("DefaultRenderCom");

        defaultRenderCom->SetTexture(CoreAsset::TextureManager::GetInstance()->GetAsset(1));
        defaultRenderCom->SetTemplateMaterial(1);
        defaultRenderCom->SetActiveState(true);

        UI::UIResizeGizmoRenderableComponent *resizeGizmoCom =
            defaultUIElement->CreateUIComponent<UI::UIResizeGizmoRenderableComponent>("ResizeGizmoCom");

        UI::UIMovableComponent *movableCom = defaultUIElement->CreateUIComponent<UI::UIMovableComponent>("moveCom");

        UI::UIButton *defaultButton1 = mUICanvas2->CreateUIElement<UI::UIButton>("DefaltUIButton");
    }

    mImGuiLogicalWindow.AddUICanvas(mUICanvas);
    mSubLogicalWindow.AddUICanvas(mUICanvas2);

    InitViewportControllers();

    mWindow->SetMaxClientWidth(1800);
    mWindow->SetMaxClientHeight(1200);

    mLogicalWindowList.push_back(&mImGuiLogicalWindow);
    mLogicalWindowList.push_back(&mSubLogicalWindow);
}

void Quad::SuperFrameController::PreUpdate() {}

void Quad::SuperFrameController::Update(float deltaTime)
{

    // ui update
    mUICanvas->Update(deltaTime);
}

void Quad::SuperFrameController::EndUpdate()
{

    UIRenderItemBuilder *uiRenderItemBuilder = UIRenderItemBuilder::GetInstance();
    //  uiRenderItemBuilder->SubmitUIElement(mUICanvas, mImGuiRenderChannelID);
    uiRenderItemBuilder->SubmitUIElement(mUICanvas, mImGuiLogicalWindow.GetRenderChannelID(),
                                         mImGuiLogicalWindow.mViewportController);
    uiRenderItemBuilder->SubmitUIElement(mUICanvas2, mSubLogicalWindow.GetRenderChannelID(),
                                         mSubLogicalWindow.mViewportController);
}

void Quad::SuperFrameController::Draw()
{

    Render::IRenderSystem *renderSystem = Render::IRenderSystem::GetInstance();

    renderSystem->StartWindow(mWindow->GetWindowHandle());

    Render::FrameContext frameContext;

    frameContext.mViewport = mImGuiLogicalWindow.mViewportController.GetViewport();

    frameContext.mScissorRect.Left = frameContext.mViewport.TopLeftX;
    frameContext.mScissorRect.Top = frameContext.mViewport.TopLeftY;
    frameContext.mScissorRect.Right = frameContext.mViewport.TopLeftX + frameContext.mViewport.Width;
    frameContext.mScissorRect.Bottom = frameContext.mViewport.TopLeftY + frameContext.mViewport.Height;

    frameContext.mBackGroundColor[0] = 1.0f;
    frameContext.mBackGroundColor[1] = 0;
    frameContext.mBackGroundColor[2] = 1.0f;
    frameContext.mBackGroundColor[3] = 1.0f;

    Render::FrameContext frameContext2 = frameContext;

    // subRenderChannel
    frameContext2.mViewport = mSubLogicalWindow.mViewportController.GetViewport();

    frameContext2.mScissorRect.Left = frameContext2.mViewport.TopLeftX;
    frameContext2.mScissorRect.Top = frameContext2.mViewport.TopLeftY;
    frameContext2.mScissorRect.Right = frameContext2.mViewport.TopLeftX + frameContext2.mViewport.Width;
    frameContext2.mScissorRect.Bottom = frameContext2.mViewport.TopLeftY + frameContext2.mViewport.Height;

    frameContext2.mBackGroundColor[0] = 0.0f;
    frameContext2.mBackGroundColor[1] = 1.0f;
    frameContext2.mBackGroundColor[2] = 0.0f;
    frameContext2.mBackGroundColor[3] = 1.0f;

    // ui
    UIRenderItemBuilder *uiRenderItemBuilder = UIRenderItemBuilder::GetInstance();

    uiRenderItemBuilder->BuildAndSubmitRenderItem(mImGuiLogicalWindow.GetRenderChannelID(), frameContext.mViewport, 0,
                                                  0);

    uiRenderItemBuilder->BuildAndSubmitRenderItem(mSubLogicalWindow.GetRenderChannelID(), frameContext2.mViewport, 0,
                                                  0);

    // imguiRenderChannel
    renderSystem->BeginFrame(mImGuiLogicalWindow.GetRenderChannelID(), frameContext);

    renderSystem->EndFrame(mImGuiLogicalWindow.GetRenderChannelID());

    renderSystem->BeginFrame(mSubLogicalWindow.GetRenderChannelID(), frameContext2);

    renderSystem->EndFrame(mSubLogicalWindow.GetRenderChannelID());

    // draw
    ImGuiSystem::GetInstance()->Draw();

    renderSystem->PresentWindow(mWindow->GetWindowHandle());
}

Quad::BaseWindow *Quad::SuperFrameController::GetWindow()
{
    return mWindow;
}

void Quad::SuperFrameController::TestLButtonDownCallback()
{

    MessageBoxW(mWindow->GetWindowHandle(), L"마우스 클릭!", L"알림", MB_OK);
}

void Quad::SuperFrameController::UpdateMouseInput(MouseContext &mouseContext)
{

    UI::UIManager *uiManager = UI::UIManager::GetInstance();

    float mouseClientPosX = mouseContext.mClientPosX;
    float mouseClientPosY = mouseContext.mClientPosY;

    if (mControllerInputContext.mMouseCaptureFlag == true)
    {
        // 마우스를 캡처한상태라면 해당 논리적윈도우채널로만 보낸다.
        Core::ViewportController &viewportController =
            mControllerInputContext.mTargetLogicalWindow->mViewportController;

        Render::Viewport viewport = viewportController.GetViewport();

        viewportController.ConvertToNdc(mouseClientPosX, mouseClientPosY);
        float windowPosX = (mouseClientPosX * viewport.Width / 2);
        float windowPosY = (mouseClientPosY * viewport.Height / 2);

        if ((viewport.Width / 2) < windowPosX)
        {
            windowPosX = viewport.Width / 2;
        }

        if (windowPosX < (-viewport.Width / 2))
        {
            windowPosX = -viewport.Width / 2;
        }

        if ((viewport.Height / 2) < windowPosY)
        {
            windowPosY = viewport.Height / 2;
        }

        if (windowPosY < (-viewport.Height / 2))
        {
            windowPosY = -viewport.Height / 2;
        }

        mouseContext.mWorldPosX = windowPosX;
        mouseContext.mWorldPosY = windowPosY;

        uiManager->UpdateInputState(mouseContext, mControllerInputContext.mTargetLogicalWindow->GetUICanvasList(),
                                    mControllerInputContext.mTargetLogicalWindow->GetUICanvasNum());

        if (uiManager->IsMouseCaptureOn() == false)
        {
            // 마우스가 캡처가 풀렸다..
            mControllerInputContext.mMouseCaptureFlag = false;
            mControllerInputContext.mTargetLogicalWindow = nullptr;
            mWindow->SetMouseCapture(false);
        }

        return;
    }

    for (size_t i = 0; i < mLogicalWindowList.size(); ++i)
    {

        Render::Viewport viewport = mLogicalWindowList[i]->mViewportController.GetViewport();
        bool ret = CoreUtility::Utility::IsPointInsideRect(viewport.TopLeftX, viewport.TopLeftX + viewport.Width,
                                                           viewport.TopLeftY + viewport.Height, viewport.TopLeftY,
                                                           mouseContext.mClientPosX, mouseContext.mClientPosY);

        if (ret == false)
            continue;

        mLogicalWindowList[i]->mViewportController.ConvertToNdc(mouseClientPosX, mouseClientPosY);

        float windowPosX = (mouseClientPosX * viewport.Width / 2);
        float windowPosY = (mouseClientPosY * viewport.Height / 2);

        mouseContext.mWorldPosX = windowPosX;
        mouseContext.mWorldPosY = windowPosY;
        uiManager->UpdateInputState(mouseContext, mLogicalWindowList[i]->GetUICanvasList(),
                                    mLogicalWindowList[i]->GetUICanvasNum());

        if (uiManager->IsMouseCaptureOn())
        {
            // 마우스가 캡처됬다.
            mControllerInputContext.mMouseCaptureFlag = true;
            mControllerInputContext.mTargetLogicalWindow = mLogicalWindowList[i];
            mWindow->SetMouseCapture(true);
        }

        return;
    }
}

LRESULT CALLBACK Quad::SuperFrameController::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
    {
        return 0;
    }

    InputSystem *inputSystem = InputSystem::GetInstance();

    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        TextOutW(hdc, 10, 10, L"Hello, Windows!", 16);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

        // 기타 메시지들 추가 가능
    case WM_LBUTTONDOWN:
        inputSystem->OnMouseEvent(EMouseState::eLButtonPressed, hwnd);

        return 0;
    case WM_RBUTTONDOWN:
        inputSystem->OnMouseEvent(EMouseState::eRButtonPressed, hwnd);
        return 0;
    case WM_LBUTTONUP:
        inputSystem->OnMouseEvent(EMouseState::eLButtonReleased, hwnd);
        return 0;
    case WM_RBUTTONUP:

        inputSystem->OnMouseEvent(EMouseState::eRButtonReleased, hwnd);
        return 0;
    case WM_MOUSEMOVE:
    {

        int clientX = GET_X_LPARAM(lParam);
        int clientY = GET_Y_LPARAM(lParam);
        POINT screenPos = {clientX, clientY};
        ClientToScreen(hwnd, &screenPos);

        inputSystem->SetMouseScreenPos(screenPos.x, screenPos.y, clientX, clientY, hwnd);
    }
        return 0;

    case WM_GETMINMAXINFO:
    {
        MINMAXINFO *pMMI = reinterpret_cast<MINMAXINFO *>(lParam);

        // 최소 크기 지정
        pMMI->ptMinTrackSize.x = mWindow->GetMinClientWidth();  // 최소 가로
        pMMI->ptMinTrackSize.y = mWindow->GetMinClientHeight(); // 최소 세로

        // 최대 크기 지정
        pMMI->ptMaxTrackSize.x = mWindow->GetMaxClientWidth();  // 최대 가로
        pMMI->ptMaxTrackSize.y = mWindow->GetMaxClientHeight(); // 최대 세로

        return 0;
    }

    case WM_SIZE:
    {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        OnResizeWindow(clientRect.right, clientRect.bottom);
    }
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void Quad::SuperFrameController::OnResizeWindow(UINT clientWidth, UINT clientHeight)
{
    Render::IRenderSystem *renderSystem = Render::IRenderSystem::GetInstance();

    mWindow->SetClientWidth(clientWidth);
    mWindow->SetClientHeight(clientHeight);

    if (renderSystem != nullptr)
    {
        renderSystem->WindowResize(mWindow->GetWindowHandle());
    }

    if (mUICanvas != nullptr)
    {
        mUICanvas->SetSize({(float)clientWidth, (float)clientHeight});
    }

    // 각 채널 viewport 도 조정
    ResizeViewportControllers(clientWidth, clientHeight);
}

void Quad::SuperFrameController::InitViewportControllers()
{

    mImGuiLogicalWindow.mViewportController.SetViewportMode(Core::EViewportMode::eAnchored);

    mImGuiLogicalWindow.mViewportController.SetAnchorLeftState(true);
    mImGuiLogicalWindow.mViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
    mImGuiLogicalWindow.mViewportController.SetAnchorLeftRelValue(0.0f);

    mImGuiLogicalWindow.mViewportController.SetAnchorRightState(true);
    mImGuiLogicalWindow.mViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
    mImGuiLogicalWindow.mViewportController.SetAnchorRightRelValue(0.5f);

    mImGuiLogicalWindow.mViewportController.SetAnchorTopState(true);
    mImGuiLogicalWindow.mViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
    mImGuiLogicalWindow.mViewportController.SetAnchorTopRelValue(0.0f);

    mImGuiLogicalWindow.mViewportController.SetAnchorBottomState(true);
    mImGuiLogicalWindow.mViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mImGuiLogicalWindow.mViewportController.SetAnchorBottomRelValue(0.2f);

    mSubLogicalWindow.mViewportController.SetViewportMode(Core::EViewportMode::eAnchored);

    mSubLogicalWindow.mViewportController.SetAnchorLeftState(true);
    mSubLogicalWindow.mViewportController.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
    mSubLogicalWindow.mViewportController.SetAnchorLeftRelValue(0.5f);

    mSubLogicalWindow.mViewportController.SetAnchorRightState(true);
    mSubLogicalWindow.mViewportController.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
    mSubLogicalWindow.mViewportController.SetAnchorRightRelValue(0.0f);

    mSubLogicalWindow.mViewportController.SetAnchorTopState(true);
    mSubLogicalWindow.mViewportController.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
    mSubLogicalWindow.mViewportController.SetAnchorTopRelValue(0.0f);

    mSubLogicalWindow.mViewportController.SetAnchorBottomState(true);
    mSubLogicalWindow.mViewportController.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
    mSubLogicalWindow.mViewportController.SetAnchorBottomRelValue(0.2f);
}

void Quad::SuperFrameController::ResizeViewportControllers(UINT width, UINT height)
{

    mImGuiLogicalWindow.mViewportController.UpdateWindowSize(width, height);
    mSubLogicalWindow.mViewportController.UpdateWindowSize(width, height);
}
