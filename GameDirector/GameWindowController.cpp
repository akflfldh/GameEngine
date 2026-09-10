#include "GameWindowController.h"
#include "Application.h"
#include <RenderFrontend/RenderPipelineManager.h>
#include <Window/BaseWindow.h>
#include <vector>

Quad::GameWindowController::GameWindowController() {}

void Quad::GameWindowController::Initialize(Render::RenderPipelineManager &renderPipelineManager)
{

    mRenderPipelineManager = &renderPipelineManager;

    Core::WindowCreateDesc desc{};
    desc.mWindowClassName = "GameWindow";
    desc.mWindowName = "Game";
    desc.mWindowStyle = WS_OVERLAPPEDWINDOW;
    desc.mWindowClassStyle = CS_HREDRAW | CS_VREDRAW;
    desc.mClientWidth = 1280;
    desc.mClientHeight = 720;
    desc.mMaxClientWidth = 3840;
    desc.mMaxClientHeight = 2160;

    InitializeWindow(desc);

    ShowWindow();
}

void Quad::GameWindowController::DrawWindow()
{

    auto *app = Quad::Application::GetInstance();

    mRenderPipelineManager->Execute(GetWorkSpace()->mWindowList, GetWindow()->GetWindowHandle(),
                                    app->GetCurrentFrameIndex(), app->GetCurrentFrameFenceValue(), true,
                                    app->GetTotalFrameCount());
}

void Quad::GameWindowController::OnRenderSurfaceResize(uint32_t width, uint32_t height)
{

    if (mRenderPipelineManager)
    {
        mRenderPipelineManager->WindowResize(GetWindow()->GetWindowHandle());
    }
}
