#pragma once

#include "ProjectSettingUIController.h"
#include <memory>

namespace Core
{
class LogicalWindow;
class WorkSpace;
}
namespace UI
{
class UICanvas;
class UIElement;
class UITheme;
}

// World/Map을 만들지 않는 프로젝트 설정 전용 작업 공간이다.
// WorkSpace와 LogicalWindow만 소유하며, Canvas는 UIManager가 소유한다.
// 설정 값의 생성/저장/로드 책임은 기존 프로젝트 시스템에 유지한다.
class ProjectSettingWorkSpaceManager
{
  public:
    static ProjectSettingWorkSpaceManager *GetInstance();
    ~ProjectSettingWorkSpaceManager();
    void Initialize(Core::LogicalWindow *globalLogicalWindow, const UI::UITheme &uiTheme);
    Core::WorkSpace *GetWorkSpace() const;
    UI::UICanvas *GetCanvas() const;
    void OnWorkSpaceActive();
    void OnWorkSpaceInActive();
    void Update();
    bool AddSettingPanel(const std::string &name, UI::UIElement *panel);

  private:
    ProjectSettingWorkSpaceManager() = default;
    std::unique_ptr<Core::WorkSpace> mWorkSpace;
    std::unique_ptr<Core::LogicalWindow> mLogicalWindow;
    UI::UICanvas *mCanvas = nullptr;
    ProjectSettingUIController mUIController;
    bool mActive = false;
};
