#include "EditorDirector/SuperAssetBrowerController.h"
#include <Core/GlobalAppHelper.h>
#include <InputSystem/InputSystem.h>
#include <Window/BaseWindow.h>
#include <functional>

Quad::SuperAssetBrowerController *Quad::SuperAssetBrowerController::GetInstance()
{

    static SuperAssetBrowerController controller;
    return &controller;
}

Quad::SuperAssetBrowerController::SuperAssetBrowerController() {}

Quad::SuperAssetBrowerController::~SuperAssetBrowerController() {}

void Quad::SuperAssetBrowerController::Initialize(Render::RenderPipelineManager &renderPipelineManager)
{
    mWindow = new BaseWindow(Core::GlobalAppHelper::GetHinstance());

    mWindow->Initialize();
    mWindow->CreateWindowClass(L"AssetBrower", L"AssetBrower");

    auto inputSystem = InputSystem::GetInstance();
    /*   inputSystem->RegisterMouseAction(EMouseInput::eRButtonDown, "Asset", this,
                                        &SuperAssetBrowerController::TestRButtonDown, 0);*/
}

void Quad::SuperAssetBrowerController::Begin() {}

void Quad::SuperAssetBrowerController::PreUpdate() {}

void Quad::SuperAssetBrowerController::Update(float deltaTime) {}

void Quad::SuperAssetBrowerController::EndUpdate() {}

void Quad::SuperAssetBrowerController::Draw(Render::RenderPipelineManager &renderPipelineManager) {}

void Quad::SuperAssetBrowerController::ShutDownWindow() {}

Quad::BaseWindow *Quad::SuperAssetBrowerController::GetWindow() const
{
    return mWindow;
}

void Quad::SuperAssetBrowerController::TestRButtonDown()
{
    MessageBoxW(mWindow->GetWindowHandle(), L"마우스 R 클릭!", L"알림", MB_OK);
}

// void Quad::SuperAssetBrowerController::UpdateMouseInput(MouseContext &mouseContext) {}

void Quad::SuperAssetBrowerController::SetMouseCapture(Core::LogicalWindow *window)
{

    mWindow->SetMouseCapture(true);
}

void Quad::SuperAssetBrowerController::ReleaseMouseCapture()
{

    mWindow->SetMouseCapture(false);
}

void Quad::SuperAssetBrowerController::SetKeyboardCapture(Core::LogicalWindow *window)
{

    mWindow->SetKeyboardCapture(true);
}

void Quad::SuperAssetBrowerController::ReleaseKeyboardCapture()
{

    mWindow->SetKeyboardCapture(false);
}

std::pair<uint32_t, uint32_t> Quad::SuperAssetBrowerController::GetWindowSize() const
{
    return std::pair<int, int>();
}
