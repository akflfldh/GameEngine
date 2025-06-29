#include "Component/ComponentFactory.h"
#include"BaseComponent.h"
Quad::ComponentFactory::ComponentFactory()
{
}

Quad::ComponentFactory::~ComponentFactory()
{
}

Quad::ComponentFactory* Quad::ComponentFactory::GetInstance()
{
	static ComponentFactory instance;

	return &instance;
}

Quad::BaseComponent* Quad::ComponentFactory::CreateComponent(const std::string& componentName)
{
	auto instance = GetInstance();

	std::unordered_map<std::string, std::function<BaseComponent* ()>>::iterator it= instance->mConstructorTable.find(componentName);

	if (it == instance->mConstructorTable.end())
		return nullptr;


	auto & constructor =	it->second;
	return	constructor();
	
}

void Quad::ComponentFactory::ReleaseComponent(BaseComponent* component)
{
	if (component == nullptr)
		return;


	auto instance = GetInstance();

	std::unordered_map<std::string, std::function<void(BaseComponent*)>>::iterator it = instance->mDestructorTable.find(component->GetClassTypeName());

	if (it == instance->mDestructorTable.end())
		return ; 


	auto & destructor = it->second;
	destructor(component);


}

void Quad::ComponentFactory::RegisterComponentCtorDtor(const std::string & componentName ,const std::function<BaseComponent* ()>& ctor, const std::function<void(BaseComponent*)>& dtor)
{
	
	auto instance = GetInstance();


	if (instance->mConstructorTable.find(componentName) != instance->mConstructorTable.end())
		return;


	instance->mConstructorTable[componentName] = ctor;
	instance->mDestructorTable[componentName] = dtor;

}
