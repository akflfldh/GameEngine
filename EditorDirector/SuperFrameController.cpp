#include "EditorDirector/SuperFrameController.h"
#include <Core/Application.h>
#include <Core/WorkSpace.h>
#include <EditorDirector/EditorConfig.h>
#include <GlobalOverlayManager.h>
#include <RenderFrontend/RenderPipelineManager.h>
#include <Window/BaseWindow.h>

Quad::SuperFrameController::~SuperFrameController() {}

Quad::SuperFrameController *Quad::SuperFrameController::GetInstance()
{
    static SuperFrameController controller;

    return &controller;
}

void Quad::SuperFrameController::Initialize(Render::RenderPipelineManager &renderPipelineManager)
{

    mRenderPipelineManager = &renderPipelineManager;

#ifdef D3DX

    // Core::D3DCoreDevice *d3dCoreDevice = static_cast<Core::D3DCoreDevice *>(Core::CoreDevice::GetInstance());
    // ImGuiSystem *imGuiSystem = ImGuiSystem::GetInstance();

    // imGuiSystem->Initialize();

#endif
    Core::WindowCreateDesc windowCreateDesc;

    if (EditorConfig::GetInstance()->GetEditorMode() == EEditorMode::eProjectBrowser)
    {

        windowCreateDesc.mMaxClientHeight = 3000;
        windowCreateDesc.mMaxClientWidth = 3000;
        windowCreateDesc.mClientHeight = 1150;
        windowCreateDesc.mClientWidth = 1000;
        windowCreateDesc.mWindowClassName = "FrameWindow";
        windowCreateDesc.mWindowName = "Editor";
        windowCreateDesc.mWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        windowCreateDesc.mWindowClassStyle = CS_HREDRAW | CS_VREDRAW;
    }
    else
    {
        // 최대크기
        windowCreateDesc.mMaxClientHeight = 3000;
        windowCreateDesc.mMaxClientWidth = 3000;
        windowCreateDesc.mClientHeight = 1150;
        windowCreateDesc.mClientWidth = 1000;
        windowCreateDesc.mWindowClassName = "FrameWindow";
        windowCreateDesc.mWindowName = "Editor";
        windowCreateDesc.mWindowStyle = WS_OVERLAPPEDWINDOW;
        windowCreateDesc.mWindowClassStyle = CS_HREDRAW | CS_VREDRAW;
    }
    WindowedFrameController::InitializeWindow(windowCreateDesc);

    if (EditorConfig::GetInstance()->GetEditorMode() == EEditorMode::eEditProject)
    {
        MaximizedWindow();

        RECT windowClientRect2;
        Quad::BaseWindow *window = GetWindow();
        GetClientRect(window->GetWindowHandle(), &windowClientRect2);

        window->SetMaxClientWidth(windowClientRect2.right);
        window->SetMaxClientHeight(windowClientRect2.bottom);
        window->SetClientWidth(windowClientRect2.right);
        window->SetClientHeight(windowClientRect2.bottom);
    }
    else
    {
        ShowWindow();
    }
}

void Quad::SuperFrameController::Begin()
{
    Core::WindowedFrameController::Begin();
}

void Quad::SuperFrameController::PreUpdate()
{

    Core::WindowedFrameController::PreUpdate();
}

void Quad::SuperFrameController::Update(float deltaTime)
{
    WindowedFrameController::Update(deltaTime);
}

void Quad::SuperFrameController::EndUpdate()
{

    Core::WindowedFrameController::EndUpdate();
}

void Quad::SuperFrameController::OnRenderSurfaceResize(uint32_t width, uint32_t height)
{

    if (mRenderPipelineManager)
    {
        mRenderPipelineManager->WindowResize(GetWindow()->GetWindowHandle());
    }
}

void Quad::SuperFrameController::OnMouseDownOutsideOverlay()
{

    GlobalOverlayManager::GetInstance()->CloseCurrentContextMenuAll();
}

void Quad::SuperFrameController::DrawWindow()
{

    auto app = Quad::Application::GetInstance();
    mRenderPipelineManager->Execute(GetWorkSpace()->mWindowList, GetWindow()->GetWindowHandle(),
                                    app->GetCurrentFrameIndex(), app->GetCurrentFrameFenceValue(), true,
                                    Application::GetInstance()->GetTotalFrameCount());
}

Quad::SuperFrameController::SuperFrameController() {}
