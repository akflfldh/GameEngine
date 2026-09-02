#pragma once

#include <EditorDirector/ClassGenerator.h>
#include <EditorDirector/IEditorTaskManager.h>
#include <string>

class ClassGenerationTaskUIController;

namespace UI
{
class UICanvas;
}

namespace Quad
{

class ClassGenerationManager : public IEditorTaskManager
{
  public:
    static ClassGenerationManager *GetInstance();

    ClassGenerationManager();
    ~ClassGenerationManager();

    void Initialize(UI::UICanvas *canvas);
    void Update() override;

    void RequestClassGeneration(const std::string &className, const std::string &parentClassName, bool isCoreClass);

  private:
    void UpdateUI(const ClassGenerationEvent &generationEvent);

  private:
    ClassGenerator *mClassGenerator = nullptr;
    ClassGenerationTaskUIController *mUIController = nullptr;

    ClassGenerationTaskHandle mGenerationTaskHandle;
    ClassGenerationEvent mGenerationEvent;
};
} // namespace Quad
