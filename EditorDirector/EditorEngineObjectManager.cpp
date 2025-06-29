#include "EditorEngineObjectManager.h"

#include"ObjectFactory/RuntimeObjectFactory.h"

Quad::EditorEngineObjectManager* Quad::EditorEngineObjectManager::GetInstance()
{
    static EditorEngineObjectManager objectManager;
    return &objectManager;
}

Quad::EditorEngineObjectManager::EditorEngineObjectManager()
    :BaseObjectManager(RuntimeObjectFactory::GetInstance())
{


}

Quad::EditorEngineObjectManager::~EditorEngineObjectManager()
{


}
