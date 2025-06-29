#include "EditorRuntimeUserObjectManager.h"


#include"ObjectFactory/RuntimeObjectFactory.h"

Quad::EditorRuntimeUserObjectManager::EditorRuntimeUserObjectManager()
	:BaseObjectManager(RuntimeObjectFactory::GetInstance())
{


}

Quad::EditorRuntimeUserObjectManager* Quad::EditorRuntimeUserObjectManager::GetInstance()
{
	static EditorRuntimeUserObjectManager objectManager;

	return &objectManager;
}

Quad::EditorRuntimeUserObjectManager::~EditorRuntimeUserObjectManager()
{


}
