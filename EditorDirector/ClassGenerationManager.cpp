#include "ClassGenerationManager.h"
#include <EditorDirector/ClassGenerator.h>
#include <EditorDirector/TaskUIController.h>

Quad::ClassGenerationManager *Quad::ClassGenerationManager::GetInstance()
{
    static ClassGenerationManager instance;
    return &instance;
}

Quad::ClassGenerationManager::ClassGenerationManager() {}

Quad::ClassGenerationManager::~ClassGenerationManager() {}

void Quad::ClassGenerationManager::Initialize(UI::UICanvas *canvas)
{
    mClassGenerator = ClassGenerator::GetInstance();
    mUIController = ClassGenerationTaskUIController::GetInstance();

    mUIController->Initialize(canvas);
}

void Quad::ClassGenerationManager::Update()
{

    if (mGenerationTaskHandle.TryPop(mGenerationEvent))
    {
        UpdateUI(mGenerationEvent);

        if (mGenerationEvent.mType == EClassGenerationEventType::eProgress)
        {
        }
        else if (mGenerationEvent.mType == EClassGenerationEventType::eCompleted)
        {

            // PostProces
            SetActiveState(false);
        }
        else if (mGenerationEvent.mType == EClassGenerationEventType::eFailed)
        {

            SetActiveState(false);
        }
    }
}

void Quad::ClassGenerationManager::RequestClassGeneration(const std::string &className,
                                                          const std::string &parentClassName, bool isCoreClass)
{
    SetActiveState(true);
    mGenerationTaskHandle = mClassGenerator->RequestClassGenerate(className, parentClassName, isCoreClass);
}

void Quad::ClassGenerationManager::UpdateUI(const ClassGenerationEvent &generationEvent)
{

    mUIController->SetMessage(generationEvent.mMessage);

    if (generationEvent.mType == EClassGenerationEventType::eProgress)
        mUIController->SetProgress(generationEvent.mProgress);
}
