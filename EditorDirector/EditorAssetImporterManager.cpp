#include "EditorAssetImporterManager.h"
#include "EditorDirector/EditorAssetImportFinalizer.h"
#include "EditorDirector/EditorAssetImporterModule.h"
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

void Quad::EditorAssetImporterManager::RequestImport(const char *file, bool bEngine)
{
    SetActiveState(true);

    // pre
    PreProcess(bEngine);

    mImportTaskHandle = mImporterModule->RequestImport(file, bEngine);
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
        auto engineFolder = logicalFileSystem->GetFolder("/Engine");
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