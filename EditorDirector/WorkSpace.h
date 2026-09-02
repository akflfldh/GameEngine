#pragma once

#include <vector>
namespace Core
{
class LogicalWindow;
}

class WorkSpace
{

  public:
    WorkSpace();
    ~WorkSpace();

    void AddLogicalWindow(Core::LogicalWindow *window);

    std::vector<Core::LogicalWindow *> mLogicalWindowList;

  private:
};