#include "EditorStaticUserObjectManager.h"

#include"ObjectFactory/ObjectFactory.h"

#include"EditorCoreComponent.h"
#include"ObjectEditorComponentTableSystem.h"
#include<Object/Object.h>

Quad::EditorStaticUserObjectManager* Quad::EditorStaticUserObjectManager::GetInstance()
{
	static EditorStaticUserObjectManager objectManager;

	return &objectManager;
}

Quad::EditorStaticUserObjectManager::EditorStaticUserObjectManager()
	:BaseObjectManager(EditObjectFactory::GetInstance())
{

}

Quad::EditorStaticUserObjectManager::~EditorStaticUserObjectManager()
{

}

Quad::Object* Quad::EditorStaticUserObjectManager::CreateObject(const std::string& className, const std::string& name)
{
	Object * object=	BaseObjectManager::CreateObject(className, name);

	if (object)
	{
		RegisterEditorComponentToObject(object);
	}


	return object;
}

Quad::Object* Quad::EditorStaticUserObjectManager::CreateObjectFromFile(const std::string& className, const std::string& name, unsigned long long id)
{
	Object* object = BaseObjectManager::CreateObjectFromFile(className, name,id);

	if (object)
	{
		RegisterEditorComponentToObject(object);
	}

	return object;
}

void Quad::EditorStaticUserObjectManager::RegisterEditorComponentToObject(Object* object)
{
	ObjectEditorComponentTableSystem* objectComponentTableSystem = ObjectEditorComponentTableSystem::GetInstance();
	objectComponentTableSystem->AddComponent<EditorCoreComponent>(object->GetUniqueID());
}
