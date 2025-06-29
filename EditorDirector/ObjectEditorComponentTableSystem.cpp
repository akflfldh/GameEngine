#include "ObjectEditorComponentTableSystem.h"
#include"Component/ComponentFactory.h"

void Quad::ObjectEditorComponentTableSystem::RemoveObject(unsigned long long objectID)
{
	
	ComTableType::iterator it = mComponentTable.find(objectID);
	if (it == mComponentTable.end())
		return;

	for (auto& componentElement : it->second)
	{
		BaseComponent * component =	componentElement.second;
		ComponentFactory::ReleaseComponent(component);
	}

	mComponentTable.erase(it);

}

Quad::ObjectEditorComponentTableSystem::ObjectEditorComponentTableSystem()
{


}

Quad::BaseComponent* Quad::ObjectEditorComponentTableSystem::GetComponent(const char* componentTypeName, unsigned long long objectID)
{


	ComTableType::iterator it= mComponentTable.find(objectID);

	if (it == mComponentTable.end())
		return nullptr;

	std::unordered_map<std::string,BaseComponent*>::iterator componentIt= it->second.find(componentTypeName);


	return componentIt == it->second.end() ? nullptr : componentIt->second;

}

Quad::BaseComponent* Quad::ObjectEditorComponentTableSystem::AddComponent(const char* componentTypeName, unsigned long long objectID)
{
	ComTableType::iterator it = mComponentTable.find(objectID);

	if (it != mComponentTable.end())
	{
		std::unordered_map<std::string, BaseComponent*>::iterator componentIt = it->second.find(componentTypeName);

		if (componentIt != it->second.end())
			return nullptr;
	}

	BaseComponent* component = ComponentFactory::CreateComponent(componentTypeName);
	if (component == nullptr)
		return nullptr;

	mComponentTable[objectID][componentTypeName] = component;

	return component;
}

Quad::ObjectEditorComponentTableSystem* Quad::ObjectEditorComponentTableSystem::GetInstance()
{
	static ObjectEditorComponentTableSystem system;

	return &system;
}

Quad::ObjectEditorComponentTableSystem::~ObjectEditorComponentTableSystem()
{

	for (auto& tableElement : mComponentTable)
	{
		for (auto& componentElement : tableElement.second)
		{
			ComponentFactory::ReleaseComponent(componentElement.second);
		}
	}

}


