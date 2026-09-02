#pragma once

/*
ImportManager,
GenerationClassManager 등의  인터페이스 클래스

*/
namespace Quad
{

class IEditorTaskManager
{
  public:
    IEditorTaskManager();
    virtual ~IEditorTaskManager() = 0;

    virtual void Update() = 0;

    virtual void SetActiveState(bool flag);
    bool GetActiveState() const;

  private:
    bool mActive = false;
};

} // namespace Quad