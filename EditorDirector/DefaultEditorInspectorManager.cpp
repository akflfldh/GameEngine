#include "DefaultEditorInspectorManager.h"
#include <IInspector.h>

DefaultEditorInspectorManager *DefaultEditorInspectorManager::GetInstance()
{
    static DefaultEditorInspectorManager instance;
    return &instance;
}

DefaultEditorInspectorManager::DefaultEditorInspectorManager() {}

DefaultEditorInspectorManager::~DefaultEditorInspectorManager() {}

void DefaultEditorInspectorManager::BeginInspectorUI()
{

    for (auto &e : mInsepctorTable)
    {
        if (e.second)
        {
            e.second->BeginUI();
        }
    }
}

void DefaultEditorInspectorManager::ActivateInsepctor(EDefaultEditorInspectorType type,
                                                      const InspectorContext &inspectorContex)
{
    if (mCurrentInspectorType == type)
        return;

    if (mCurrentInspectorType != EDefaultEditorInspectorType::eNone)
    {
        auto it = mInsepctorTable.find(mCurrentInspectorType);

        if (it != mInsepctorTable.end())
        {
            it->second->DeActivateInspector();
        }
    }

    mCurrentInspectorType = type;

    if (mCurrentInspectorType != EDefaultEditorInspectorType::eNone)
    {
        auto it = mInsepctorTable.find(mCurrentInspectorType);
        if (it != mInsepctorTable.end())
        {
            it->second->SetInspectorContext(inspectorContex);
            it->second->ActivateInspector();
        }
    }
}

void DefaultEditorInspectorManager::RegisterInspector(EDefaultEditorInspectorType type, IInspector *inspector)
{

    mInsepctorTable[type] = inspector;
}
