#pragma once

#include <vector>

#include <Core/CoreDllExport.h>

namespace Core
{

class LogicalWindow;

class CORE_API_LIB WorkSpace
{
  public:
    WorkSpace();
    ~WorkSpace();

    void AddLogicalWindow(LogicalWindow *window);
    void SetGlobalOverlayWindow(LogicalWindow *window);

    void OnResizeWindow(uint32_t width, uint32_t height);

    void UpdateFrameData();

    LogicalWindow *FindLogicalWindowAtClientPos(int x, int y) const;
    LogicalWindow *mGloberOverlayWindow = nullptr;
    std::vector<LogicalWindow *> mWindowList;

  private:
};

} // namespace Core