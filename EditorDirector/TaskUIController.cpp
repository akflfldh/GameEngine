#include "TaskUIController.h"
#include <EditorDirector/UIBoolPanel.H>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UIVerticalLayoutComponent.h>

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

ProjectBuildTaskUIController *ProjectBuildTaskUIController::GetInstance()
{
    static ProjectBuildTaskUIController instance;

    return &instance;
}

ProjectBuildTaskUIController::ProjectBuildTaskUIController() {}

ProjectBuildTaskUIController::~ProjectBuildTaskUIController() {}

void ProjectBuildTaskUIController::Initialize(UI::UICanvas *canvas)
{
    if (canvas == nullptr)
        return;

    mBackgroundUI = canvas->CreateUIElement<UI::UIImage>("Background");
    mProgressText = mBackgroundUI->CreateChildUIElement<UI::UIText>("ProgressText");

    mBackgroundUI->SetSize(400, 250);
    mBackgroundUI->SetColor(0.4f, 0.4f, 0.4f);

    mBackgroundUI->SetActiveFlag(false);
}

void ProjectBuildTaskUIController::Update() {}

void ProjectBuildTaskUIController::SetActive(bool flag) {}

void ProjectBuildTaskUIController::SetPosition(float x, float y) {}

void ProjectBuildTaskUIController::RequestKeyboardCapture() {}

void ProjectBuildTaskUIController::RequestMouseCapture() {}

void ProjectBuildTaskUIController::ReleaseKeyboardCapture() {}

void ProjectBuildTaskUIController::ReleaseMouseCapture() {}

void ProjectBuildTaskUIController::SetMessage(const std::string &message)
{

    if (mProgressText == nullptr)
        return;

    mProgressText->SetText(message);
}
void ProjectBuildTaskUIController::SetProgress(float progress)
{

    // progress image
}

void ProjectBuildTaskUIController::CreateExitButton(UI::UIElement *parent) {}
void ProjectBuildTaskUIController::OnClickedExitButton() {}

ImportTaskSettingUIController *ImportTaskSettingUIController::GetInstance()
{

    static ImportTaskSettingUIController instance;

    return &instance;
}

ImportTaskSettingUIController::ImportTaskSettingUIController() {}

ImportTaskSettingUIController::~ImportTaskSettingUIController() {}

void ImportTaskSettingUIController::Initialize(UI::UICanvas *canvas)
{

    if (canvas == nullptr)
        return;

    mBackgroundUI = canvas->CreateUIElement<UI::UIImage>("Background");
    mBackgroundUI->SetStyleRole(UI::EUIStyleRole::ePanel);
    UI::UIControlStyleOverride backgroundStyleOverride;
    backgroundStyleOverride.mHeight = 1000;
    mBackgroundUI->SetStyleOverride(backgroundStyleOverride);
    float backgroundWidth = 600.0f;
    mBackgroundUI->SetWidth(backgroundWidth);

    mBackgroundUI->SetActiveFlag(false);

    mBackgroundUI->SetPositionLocal(200, 100);

    mContentUIPanel = mBackgroundUI->CreateChildUIElement<UI::UIImage>("ContentPanel");
    mContentUIPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayout");
    mContentUIPanel->SetStyleRole(UI::EUIStyleRole::ePanel);
    backgroundStyleOverride.mHeight = 1000;
    mContentUIPanel->SetStyleOverride(backgroundStyleOverride);
    mContentUIPanel->SetWidth(backgroundWidth);
    mContentUIPanel->SetActiveFlag(true);

    auto headerBarPanel = mContentUIPanel->CreateChildUIElement<UI::UIImage>("HeaderBarPanel");
    headerBarPanel->SetStyleRole(UI::EUIStyleRole::eSectionHeader);
    headerBarPanel->SetWidth(backgroundWidth);

    mExitButton = headerBarPanel->CreateChildUIElement<UI::UIButton>("ExitButton");
    mExitButton->SetSize(50, 50);
    mExitButton->mUIImageComponent->UseTexture();
    mExitButton->mUIImageComponent->SetTexture("Engine/Exit");
    mExitButton->SetHorizontalPivotOffset(0.0f);
    mExitButton->SetVerticalPivotOffset(0.0f);
    mExitButton->SetHorizontalPivotSide(UI::EUIPosPivotHorizontal::eRight);
    mExitButton->SetVerticalPivotSide(UI::EUIPosPivotVertical::eTop);

    mExitButton->SetPositionLocal(mContentUIPanel->mTransform.GetSize().x - mExitButton->mTransform.GetSize().x, 0.0f);

    mExitButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float) { SetActive(false); });

    mImportButton = headerBarPanel->CreateChildUIElement<UI::UITextButton>("ImportButton");
    mImportButton->mTextComponent->SetText("임포트");
    mImportButton->SetWidth(60.0f);
    mImportButton->SetPositionLocal(mExitButton->mTransform.GetLocalPosition().x - mImportButton->GetWidth() - 10.0f,
                                    0);
    mImportButton->mUIImageComponent->SetUseBorderFlag(true);

    mImportButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float) { OnClickedImportRequestButtonCallbackSystem(); });

    CreateTextureSettingPanel();

    CloseAllAssetPanel();
    SetActive(false);
}

void ImportTaskSettingUIController::Update() {}

void ImportTaskSettingUIController::SetActive(bool flag)
{

    mBackgroundUI->SetActiveFlag(flag);
}

void ImportTaskSettingUIController::SetPosition(float x, float y) {}

void ImportTaskSettingUIController::RequestKeyboardCapture() {}

void ImportTaskSettingUIController::RequestMouseCapture() {}

void ImportTaskSettingUIController::ReleaseKeyboardCapture() {}

void ImportTaskSettingUIController::ReleaseMouseCapture() {}

void ImportTaskSettingUIController::OpenSettingPanel(const std::filesystem::path &file, bool bEngine)
{

    // open
    EAssetImporterType importType = GetImporterType(file);

    mBackgroundUI->SetActiveFlag(true);

    // 초기화
    mEditorImportRequest = {};
    mEditorImportRequest.mSourcePath = file;
    mEditorImportRequest.bEngine = bEngine;
    mEditorImportRequest.mAssetImportType = importType;
    // importType에맞는 자식 asset setting panel active
    CloseAllAssetPanel();

    switch (importType)
    {
    case EAssetImporterType::eTexture:
        mTextureSettingPanelUI->SetActiveFlag(true);
        break;

    case EAssetImporterType::eFbx:

        break;
    }
}

EAssetImporterType ImportTaskSettingUIController::GetImporterType(const std::filesystem::path &file) const
{

    const std::string extension = file.extension().string();

    if (extension == ".fbx")
    {
        return EAssetImporterType::eFbx;
    }
    else if (extension == ".png" || extension == ".jpg" || extension == ".bmp" || extension == "jpeg")
    {
        return EAssetImporterType::eTexture;
    }

    return EAssetImporterType::eUnknown;
}

void ImportTaskSettingUIController::CreateTextureSettingPanel()
{

    float width = mBackgroundUI->GetWidth();
    float height = mBackgroundUI->GetHeight();

    mTextureSettingPanelUI = mContentUIPanel->CreateChildUIElement<UI::UIImage>("TextureSettingPanel");
    UI::UIControlStyleOverride texturePanelStyleOverride;
    mTextureSettingPanelUI->SetStyleRole(UI::EUIStyleRole::ePanel);
    mTextureSettingPanelUI->SetStyleOverride(texturePanelStyleOverride);
    mTextureSettingPanelUI->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayout");
    texturePanelStyleOverride.mHeight = height;

    mTextureSettingPanelUI->SetWidth(width);

    auto sRGBBoolPanel = mTextureSettingPanelUI->CreateChildUIElement<UIBoolPanel>("SRGB_Panel");
    sRGBBoolPanel->SetStyleRole(UI::EUIStyleRole::ePropertyRow);
    sRGBBoolPanel->SetWidth(width);
    sRGBBoolPanel->SetTagText("SRGB");

    sRGBBoolPanel->mOnValueChanged.Register([this](bool bSRGB)
                                            { mEditorImportRequest.mTextureSettings.bSRGB = bSRGB; });
}

void ImportTaskSettingUIController::CloseAllAssetPanel()
{

    if (mTextureSettingPanelUI)
        mTextureSettingPanelUI->SetActiveFlag(false);
}

void ImportTaskSettingUIController::OnClickedImportRequestButtonCallbackSystem()
{

    SetActive(false);
    mOnClickedImportRequestButtonCallbackSystem.ExecuteCallbacks(mEditorImportRequest);
}

MaterialCreationUIController *MaterialCreationUIController::GetInstance()
{
    static MaterialCreationUIController instance;

    return &instance;
}

MaterialCreationUIController::MaterialCreationUIController() {}

MaterialCreationUIController::~MaterialCreationUIController() {}

void MaterialCreationUIController::Initialize(UI::UICanvas *canvas)
{

    if (canvas == nullptr)
        return;

    mBackgroundUI = canvas->CreateUIElement<UI::UIImage>("Background");
    mBackgroundUI->SetStyleRole(UI::EUIStyleRole::ePanel);
    UI::UIControlStyleOverride backgroundStyleOverride;
    backgroundStyleOverride.mHeight = 1000;
    mBackgroundUI->SetStyleOverride(backgroundStyleOverride);
    float backgroundWidth = 600.0f;
    mBackgroundUI->SetWidth(backgroundWidth);

    mBackgroundUI->SetActiveFlag(false);

    mBackgroundUI->SetPositionLocal(200, 100);

    mContentUIPanel = mBackgroundUI->CreateChildUIElement<UI::UIImage>("ContentPanel");
    mContentUIPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayout");
    mContentUIPanel->SetStyleRole(UI::EUIStyleRole::ePanel);
    backgroundStyleOverride.mHeight = 1000;
    mContentUIPanel->SetStyleOverride(backgroundStyleOverride);
    mContentUIPanel->SetWidth(backgroundWidth);
    mContentUIPanel->SetActiveFlag(true);

    auto headerBarPanel = mContentUIPanel->CreateChildUIElement<UI::UIImage>("HeaderBarPanel");
    headerBarPanel->SetStyleRole(UI::EUIStyleRole::eSectionHeader);
    headerBarPanel->SetWidth(backgroundWidth);

    mExitButton = headerBarPanel->CreateChildUIElement<UI::UIButton>("ExitButton");
    mExitButton->SetSize(50, 50);
    mExitButton->mUIImageComponent->UseTexture();
    mExitButton->mUIImageComponent->SetTexture("Engine/Exit");
    mExitButton->SetHorizontalPivotOffset(0.0f);
    mExitButton->SetVerticalPivotOffset(0.0f);
    mExitButton->SetHorizontalPivotSide(UI::EUIPosPivotHorizontal::eRight);
    mExitButton->SetVerticalPivotSide(UI::EUIPosPivotVertical::eTop);

    mExitButton->SetPositionLocal(mContentUIPanel->mTransform.GetSize().x - mExitButton->mTransform.GetSize().x, 0.0f);

    mExitButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float) { SetActive(false); });

    mCreationButton = headerBarPanel->CreateChildUIElement<UI::UITextButton>("CreationButton");
    mCreationButton->mTextComponent->SetText("생성");
    mCreationButton->SetWidth(60.0f);
    mCreationButton->SetPositionLocal(
        mExitButton->mTransform.GetLocalPosition().x - mCreationButton->GetWidth() - 10.0f, 0);
    mCreationButton->mUIImageComponent->SetUseBorderFlag(true);

    mCreationButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float) { OnClickedCreationMaterialRequestsButton(); });

    auto materialNameBar = mContentUIPanel->CreateChildUIElement<UI::UIImage>("MaterialNameBar");
    materialNameBar->SetStyleRole(UI::EUIStyleRole::ePropertyRow);
    materialNameBar->SetWidth(backgroundWidth);

    auto materialNameTagPanel = materialNameBar->CreateChildUIElement<UI::UIText>("MaterialNameTag");
    materialNameTagPanel->SetText("머터리얼 이름");

    mMaterialNameEditBox = materialNameBar->CreateChildUIElement<UI::UIEditBox>("MaterialNameEditBox");
    mMaterialNameEditBox->SetWidth(300.0f);
    mMaterialNameEditBox->SetBackgroundColor(1, 1, 1);
    mMaterialNameEditBox->SetTextColor(0, 0, 0);
    mMaterialNameEditBox->SetTextInputType(UI::EUITextInputType::eString);
    mMaterialNameEditBox->SetOverflowMode(UI::EUITextOverflowMode::eScrollHorizontal);
    mMaterialNameEditBox->SetClipingMode(UI::EUITextClipingMode::eScissor);

    mMaterialNameEditBox->SetPositionLocal(200.0f, 0);

    SetActive(false);
}

void MaterialCreationUIController::Update() {}

void MaterialCreationUIController::SetActive(bool flag)
{

    if (mBackgroundUI)
    {
        mBackgroundUI->SetActiveFlag(flag);
    }
}

void MaterialCreationUIController::SetPosition(float x, float y) {}

void MaterialCreationUIController::RequestKeyboardCapture() {}

void MaterialCreationUIController::RequestMouseCapture() {}

void MaterialCreationUIController::ReleaseKeyboardCapture() {}

void MaterialCreationUIController::ReleaseMouseCapture() {}

void MaterialCreationUIController::OpenPanel()
{

    // 초기화
    mMaterialNameEditBox->SetText("");

    SetActive(true);
}

void MaterialCreationUIController::OnClickedCreationMaterialRequestsButton()
{

    mOnClickedCreationMaterialRequestButtonCallbackSystem.ExecuteCallbacks(mMaterialNameEditBox->GetText());
}
