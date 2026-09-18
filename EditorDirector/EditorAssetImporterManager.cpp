#include "EditorAssetImporterManager.h"
#include "EditorDirector/EditorAssetImportFinalizer.h"
#include "EditorDirector/EditorAssetImporterModule.h"
#include <CoreAsset/IntermediateAsset.h>
#include <EditorDirector/TaskUIController.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
Quad::EditorAssetImporterManager *Quad::EditorAssetImporterManager::GetInstance()
{
    static EditorAssetImporterManager instance;
    return &instance;
}

Quad::EditorAssetImporterManager::EditorAssetImporterManager() {}

Quad::EditorAssetImporterManager::~EditorAssetImporterManager() {}

void Quad::EditorAssetImporterManager::Initialize(UI::UICanvas *canvas)
{
    mImporterModule = EditorAssetImporterModule::GetInstance();
    mImporterModule->Initialize();

    mUIController = ImportTaskUIController::GetInstance();
    mUIController->Initialize(canvas);

    mUIController->mOnClickedExitButtonCallbackSystem.Register([this]() { OnClickedExitButton(); });

    mSettingUIController = ImportTaskSettingUIController::GetInstance();
    mSettingUIController->Initialize(canvas);
    mSettingUIController->mOnClickedImportRequestButtonCallbackSystem.Register(
        [this](const EditorImportRequest &importRequest) { StartImport(importRequest); });

    mFinalizer = EditorAssetImportFinalizer::GetInstance();
}

void Quad::EditorAssetImporterManager::Update()
{

    if (mImportTaskHandle.TryPopEvent(mImportEvent))
    {
        UpdateUI(mImportEvent);

        if (mImportEvent.mType == EImportEventType::eProgress)
        {
            int a = 2;
        }
        else if (mImportEvent.mType == EImportEventType::eCompleted)
        {

            // post
            PostProcess(mImportEvent.mResult);
            SetActiveState(false);
        }
        else if (mImportEvent.mType == EImportEventType::eFailed)
        {

            // post
            PostProcess(mImportEvent.mResult);
            SetActiveState(false);
        }
    }
}

void Quad::EditorAssetImporterManager::RequestImport(const std::filesystem::path file, bool bEngine)
{
    // SetActiveState(true);

    // 파일확장자로 asset 타입 확인

    // Importer setthing 창 활성화
    mSettingUIController->OpenSettingPanel(file, bEngine);
    // 또는 임포트 요청버튼에 콜백등록
    //  사용자가 설정

    // 설정값을 rquest로 복사(snapshot)
}

ImportResult Quad::EditorAssetImporterManager::RequestImportSync(const std::filesystem::path &file, bool bEngine)
{
    // pre
    PreProcess(bEngine);
    ImportResult result = mImporterModule->RequestImportSync(file, bEngine);

    // post
    PostProcess(result);

    return result;
}

ImportResult Quad::EditorAssetImporterManager::RequestImportSync(const EngineAssetImportRequest &importRequest)
{

    PreProcess(true);

    CoreAsset::AssetImportContext importContext;
    importContext.mEngineAsset = true;
    importContext.mRegistryPrefix = importRequest.mTargetRegistryPath;
    CoreAsset::AssetCreationContext creationContext;
    creationContext.mRequestedAssetID = importRequest.mRequestedAssetID;
    creationContext.mRequestedAssetName = importRequest.mRequestedAssetName;

    importContext.mCreationContextTable[importRequest.mTargetImportAssetKey] = creationContext;

    ImportResult result = mImporterModule->RequestImportSync(importRequest.mSourcePath, importContext);

    // post
    PostProcess(result);
    return result;
}

ImportResult Quad::EditorAssetImporterManager::ImportDedencySync(const char *file, bool bEngine)
{
    return mImporterModule->ImportDendencySync(file, bEngine);
}

void Quad::EditorAssetImporterManager::SetActiveState(bool flag)
{

    IEditorTaskManager::SetActiveState(flag);

    if (mUIController)
    {
        mUIController->SetActive(flag);
    }
}

void Quad::EditorAssetImporterManager::PreProcess(bool bEngine)
{

    QuadLF::LogicalFileSystem *logicalFileSystem = QuadLF ::LogicalFileSystem::GetInstance();

    mPreLogicalFolder = logicalFileSystem->GetCurrentLogicalFolder();
    if (bEngine)
    {
        auto engineFolder = logicalFileSystem->GetFolder("Engine");
        logicalFileSystem->SetCurrentLogicalFolder(engineFolder);
    }
}
void Quad::EditorAssetImporterManager::PostProcess(const ImportResult &importResult)
{

    if (importResult.mSuccess)
        mFinalizer->Finalize(importResult);

    QuadLF::LogicalFileSystem *logicalFileSystem = QuadLF ::LogicalFileSystem::GetInstance();

    if (mPreLogicalFolder)
    {
        logicalFileSystem->SetCurrentLogicalFolder(mPreLogicalFolder);
        mPreLogicalFolder = nullptr;
    }
}

void Quad::EditorAssetImporterManager::UpdateUI(const ImportEvent &importEvent)
{

    mUIController->SetMessage(importEvent.mMessage);

    if (importEvent.mType == EImportEventType::eProgress)
        mUIController->SetProgress(importEvent.mProgress);
}

void Quad::EditorAssetImporterManager::OnClickedExitButton()
{
    SetActiveState(false);
    // 마우스캡처 풀기 등등
}

void Quad::EditorAssetImporterManager::StartImport(const EditorImportRequest &importRequest)
{

    // 비동기 임포트
    SetActiveState(true);

    // pre
    PreProcess(importRequest.bEngine);

    // request정보를 넘긴다.
    mImportTaskHandle = mImporterModule->RequestImport(importRequest);
}