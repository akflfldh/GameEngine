#include "TaskUIController.h"
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>

ITaskUIController::ITaskUIController() {}

ITaskUIController::~ITaskUIController() {}

ClassGenerationTaskUIController *ClassGenerationTaskUIController::GetInstance()
{
    static ClassGenerationTaskUIController instance;
    return &instance;
}

ClassGenerationTaskUIController::ClassGenerationTaskUIController() : mBackgroundUI(nullptr), mProgressText(nullptr) {}

ClassGenerationTaskUIController::~ClassGenerationTaskUIController() {}

void ClassGenerationTaskUIController::Initialize(UI::UICanvas *canvas)
{
    mBackgroundUI = canvas->CreateUIElement<UI::UIImage>("Background");
    mProgressText = mBackgroundUI->CreateChildUIElement<UI::UIText>("ProgressText");

    mBackgroundUI->SetSize(400, 250);
    mBackgroundUI->SetColor(0.4f, 0.4f, 0.4f);

    mProgressText->SetFontSize(16.0f);
    mProgressText->SetTextColor({1, 1, 1});
    mProgressText->SetSize(300, 25);
    mProgressText->SetPositionLocal(20, 200);
    mProgressText->SetText("파일 생성 중");

    CreateExitButton(mBackgroundUI);

    SetActive(false);
}

void ClassGenerationTaskUIController::Update() {}

void ClassGenerationTaskUIController::SetMessage(const std::string &message) {}

void ClassGenerationTaskUIController::SetProgress(float progress) {}

void ClassGenerationTaskUIController::SetActive(bool flag)
{
    if (mBackgroundUI)
    {
        mBackgroundUI->SetActiveFlag(flag);
    }

    if (flag)
    {
        RequestMouseCapture();
    }
    else
    {
        ReleaseMouseCapture();
    }
}

void ClassGenerationTaskUIController::SetPosition(float x, float y)
{
    if (mBackgroundUI)
    {
        mBackgroundUI->SetPositionLocal(x, y);
    }
}

void ClassGenerationTaskUIController::RequestKeyboardCapture()
{
    if (mBackgroundUI)
    {
        mBackgroundUI->RequestKeyboardCaptureInput(nullptr);
    }
}

void ClassGenerationTaskUIController::RequestMouseCapture()
{
    if (mBackgroundUI)
    {
        mBackgroundUI->RequestMouseCaptureInput(nullptr);
    }
}

void ClassGenerationTaskUIController::ReleaseKeyboardCapture()
{
    if (mBackgroundUI)
    {
        mBackgroundUI->ReleaseKeyboardCaptureInput();
    }
}

void ClassGenerationTaskUIController::ReleaseMouseCapture()
{
    if (mBackgroundUI)
    {
        mBackgroundUI->ReleaseMouseCaptureInput();
    }
}

void ClassGenerationTaskUIController::CreateExitButton(UI::UIElement *parentElement)
{
    mExitButton = parentElement->CreateChildUIElement<UI::UIButton>("ExitButton");
    mExitButton->SetSize(50, 50);
    mExitButton->mUIImageComponent->UseTexture();
    mExitButton->mUIImageComponent->SetTexture("Engine/Exit");
    mExitButton->SetHorizontalPivotOffset(0.0f);
    mExitButton->SetVerticalPivotOffset(0.0f);
    mExitButton->SetHorizontalPivotSide(UI::EUIPosPivotHorizontal::eRight);
    mExitButton->SetVerticalPivotSide(UI::EUIPosPivotVertical::eTop);

    mExitButton->mHoverCallbackSystem.Register([this](float, float) { OnClickedExitButton(); });
}

void ClassGenerationTaskUIController::OnClickedExitButton()
{
    mOnClickedExitButtonCallbackSystem.ExecuteCallbacks();
}

ImportTaskUIController *ImportTaskUIController::GetInstance()
{

    static ImportTaskUIController instance;

    return &instance;
}

ImportTaskUIController::ImportTaskUIController() {}

ImportTaskUIController::~ImportTaskUIController() {}

void ImportTaskUIController::Initialize(UI::UICanvas *canvas)
{

    if (canvas == nullptr)
        return;

    mBackgroundUI = canvas->CreateUIElement<UI::UIImage>("BackgroundImage");
    mBackgroundUI->SetSize(400, 250);
    mBackgroundUI->SetColor(0.4f, 0.4f, 0.4f);
    mBackgroundUI->SetPositionLocal(300, 300);

    mProgressText = mBackgroundUI->CreateChildUIElement<UI::UIText>("ProgressText");
    mProgressText->SetSize(300, 30);
    mProgressText->SetFontSize(20.0f);
    mProgressText->SetTextColor({0, 0, 0});

    mProgressText->SetPositionLocal(20, 20);

    CreateExitButton(mBackgroundUI);

    SetActive(false);
}

void ImportTaskUIController::SetActive(bool flag)
{

    if (mBackgroundUI)
    {
        mBackgroundUI->SetActiveFlag(flag);
    }

    if (flag)
    {
        // 마우스,키보드를 background 범위 캡처로
        RequestMouseCapture();
    }
    else
    {
        // 캡처 해제
        ReleaseMouseCapture();
    }
}

void ImportTaskUIController::SetPosition(float x, float y) {}

void ImportTaskUIController::RequestKeyboardCapture() {}

void ImportTaskUIController::RequestMouseCapture()
{

    if (mBackgroundUI)
    {
        mBackgroundUI->RequestMouseCaptureInput(nullptr);
    }
}

void ImportTaskUIController::ReleaseKeyboardCapture() {}

void ImportTaskUIController::ReleaseMouseCapture()
{

    if (mBackgroundUI)
    {
        mBackgroundUI->ReleaseMouseCaptureInput();
    }
}

void ImportTaskUIController::Update() {}

void ImportTaskUIController::SetMessage(const std::string &message)
{

    if (mProgressText == nullptr)
        return;

    mProgressText->SetText(message);
}
void ImportTaskUIController::SetProgress(float progress)
{

    // progress image
}

void ImportTaskUIController::CreateExitButton(UI::UIElement *parent)
{

    mExitButton = parent->CreateChildUIElement<UI::UIButton>("ExitButton");
    mExitButton->SetSize(50, 50);
    mExitButton->mUIImageComponent->UseTexture();
    mExitButton->mUIImageComponent->SetTexture("Engine/Exit");
    mExitButton->SetHorizontalPivotOffset(0.0f);
    mExitButton->SetVerticalPivotOffset(0.0f);
    mExitButton->SetHorizontalPivotSide(UI::EUIPosPivotHorizontal::eRight);
    mExitButton->SetVerticalPivotSide(UI::EUIPosPivotVertical::eTop);

    mExitButton->SetPositionLocal(mBackgroundUI->mTransform.GetSize().x - mExitButton->mTransform.GetSize().x, 0.0f);

    mExitButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float)
                                                                         { OnClickedExitButton(); });
}

void ImportTaskUIController::OnClickedExitButton()
{

    mOnClickedExitButtonCallbackSystem.ExecuteCallbacks();
}
