#pragma once

#include <string>
#include <unordered_map>

namespace UI
{
class UICanvas;
}

class UIProjectBrowser;

class EditorProjectBrowserManager
{

  public:
    static EditorProjectBrowserManager *GetInstance();
    EditorProjectBrowserManager();
    ~EditorProjectBrowserManager();

    void Initialize(UI::UICanvas *uiCanvas);

  private:
    void InitProjectList();
    void CreateProject(const std::string &projectPath, const std::string &projectName);

    bool VerifyProjectPath(const std::string &projectPath);

    void SaveProjectListFile();

    void OnSelectedProject(const std::string &projectName);

    bool LaunchNewEditor(const std::string &projectName, const std::string &projectPath);

  private:
    UIProjectBrowser *mUIProjectBrowser;
    std::unordered_map<std::string, std::string> mProjectPathTable;
};
