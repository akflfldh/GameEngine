#pragma once

#include <EditorDirector/IEditorTaskManager.h>
#include <EditorDirector/ProjectBuildCommonHeader.h>
#include <filesystem>

namespace UI
{
class UICanvas;
}
class ProjectBuildTaskUIController;

namespace Quad
{

class EditorBuildManager : public IEditorTaskManager
{
  public:
    static EditorBuildManager *GetInstance();
    EditorBuildManager();
    virtual ~EditorBuildManager();

    void Initialize(UI::UICanvas *canvas);
    virtual void Update() override;

    bool RequestBuildProject(const std::filesystem::path &path);

  private:
    // 빌드 수행전 빌드에필요한 정보 수집
    bool BuildRequestSnapshot(const std::filesystem::path &outputDirectory, ProjectBuildRequest &oRequest);
    bool AppendRuntimeEngineAssetRecords(std::vector<PakWriteAssetRecord> &oRecords) const;

    // 작업스레드 빌드 작업 제출
    void ExecuteBuild(ProjectBuildJobContext jobContext, const ProjectBuildRequest &projectBuildRequest);

    void HandleBuildEvent(const ProjectBuildEvent &event);
    void FinishBuild(bool success);

    void UpdateUI();

    void OnClickedExitButton();

  private:
    ProjectBuildTaskHandle mTaskHandle;
    ProjectBuildEvent mCurrentEvent;
    ProjectBuildTaskUIController *mUIController = nullptr;
};
} // namespace Quad
