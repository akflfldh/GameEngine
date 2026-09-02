#pragma once

#include <EditorDirector/IEditorTaskManager.h>
#include <EditorDirector/ImportCommonHeader.h>
#include <filesystem>
// #include <EditorDirector/TaskUIController.h>

class ClassGenerationTaskUIController;
class ImportTaskUIController;
namespace UI
{
class UICanvas;
}

namespace QuadLF
{
class LogicalFolder;
}

namespace Quad
{
class EditorAssetImporterModule;
class EditorAssetImportFinalizer;

class EditorAssetImporterManager : public IEditorTaskManager
{
  public:
    static EditorAssetImporterManager *GetInstance();

    EditorAssetImporterManager();
    virtual ~EditorAssetImporterManager();

    void Initialize(UI::UICanvas *canvas);
    virtual void Update() override;

    // 비동기버전
    void RequestImport(const char *file, bool bEngine = false);

    // 동기 버전 (ui처리가없는) (호출스레드가 직접수행 )
    ImportResult RequestImportSync(const std::filesystem::path &file, bool bEngine = false);

    // import내에서 중첩호출버전 (ui처리 x)(호출 스레드가 직접수행)
    ImportResult ImportDedencySync(const char *file, bool bEngine = false);

    virtual void SetActiveState(bool flag) override;

  private:
    void PreProcess(bool bEngine);
    void PostProcess(const ImportResult &importResult);

    void UpdateUI(const ImportEvent &importEvent);

    void OnClickedExitButton();

  private:
    EditorAssetImporterModule *mImporterModule = nullptr;
    EditorAssetImportFinalizer *mFinalizer = nullptr;
    ImportTaskUIController *mUIController = nullptr;
    ImportTaskHandle mImportTaskHandle;
    ImportEvent mImportEvent;

    QuadLF::LogicalFolder *mPreLogicalFolder = nullptr;
};

} // namespace Quad