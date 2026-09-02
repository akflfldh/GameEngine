#include "IEditorTaskManager.h"

Quad::IEditorTaskManager::IEditorTaskManager() {}

Quad::IEditorTaskManager::~IEditorTaskManager() {}

void Quad::IEditorTaskManager::SetActiveState(bool flag)
{

    mActive = flag;
}

bool Quad::IEditorTaskManager::GetActiveState() const
{

    return mActive;
}
