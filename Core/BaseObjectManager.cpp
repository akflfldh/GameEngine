#include "ObjectManager/BaseObjectManager.h"
#include"ObjectFactory/BaseObjectFactory.h"
#include"Object/Object.h"
#include"Map/Map.h"
#include<sstream>


Quad::BaseObjectManager::BaseObjectManager(BaseObjectFactory* objectFactory)
	:mObjectFactory(objectFactory)
{
}

Quad::BaseObjectManager::~BaseObjectManager()
{
}



Quad::Object* Quad::BaseObjectManager::CreateObject(const std::string& className, const std::string& name)
{
	
	if (mNameTable.GetID(name) != 0)
		return nullptr;


	Object* object = mObjectFactory->CreateObject(className);

	mUniqueIDTable.Register(object);
	mNameTable.Register(name, object->GetUniqueID());

	//object->SetName(name);

	return object;
	

}


Quad::Object* Quad::BaseObjectManager::CreateObjectFromFile(const std::string& className, const std::string& name, unsigned long long id)
{

	if (mNameTable.GetID(name) != 0 || mUniqueIDTable.GetElement(id) != nullptr)
		return nullptr;


	Object* object = mObjectFactory->CreateObject(className);
	object->SetName(name);
	mUniqueIDTable.RegisterFromFile(object, id);
	mNameTable.Register(name, id);

	
	return object;
}


void Quad::BaseObjectManager::KillObject(Object* object)
{
	mDeadObjectTable.push_back(object);
}

void Quad::BaseObjectManager::RemoveDeadObject()
{
	for (auto element : mDeadObjectTable)
	{
		Map* map = element->GetMap();
		map->RemoveObject(element);

		ReleaseObject(element);
	}
	mDeadObjectTable.resize(0);
}

Quad::Object* Quad::BaseObjectManager::GetObjectFromID(unsigned long long id)
{

	//auto instance = GetInstance();
	//if (id == 0)
	//	return nullptr;

	//return instance->mUniqueIDTable.GetElement(id);

	
	if (id == 0)
		return nullptr;

	return mUniqueIDTable.GetElement(id);

}

Quad::Object* Quad::BaseObjectManager::GetObjectFromName(const std::string& name)
{
	/*auto instance = GetInstance();

	unsigned long long id = instance->mNameTable.GetID(name);


	return GetObjectFromID(id);*/


	

	unsigned long long id = mNameTable.GetID(name);


	return GetObjectFromID(id);

}

bool Quad::BaseObjectManager::ChangeObjectName(unsigned long long id, const std::string& beforeName, const std::string& newName)
{
	/*if (id == 0)
		return false;

	auto instance = GetInstance();
	instance->mNameTable.UnRegister(beforeName);
	return instance->mNameTable.Register(newName, id);*/

	if (id == 0)
		return false;


	mNameTable.UnRegister(beforeName);
	return mNameTable.Register(newName, id);

}

unsigned long long Quad::BaseObjectManager::PeekObjectUniqueID() const
{
	return mUniqueIDTable.PeekUniqueID();
}

void Quad::BaseObjectManager::SetNextAvailableObjectUniqueID(unsigned long long id)
{

	mUniqueIDTable.SetNextAvailalbeUniqueID(id);

}

int Quad::BaseObjectManager::GetNameTableElementNum() const
{
	return mNameTable.GetElementNum();
}

Quad::BaseObjectFactory* Quad::BaseObjectManager::GetObjectFactory() const
{
	return mObjectFactory;
}

void Quad::BaseObjectManager::ReleaseObject(Object* object)
{
	mUniqueIDTable.UnRegister(object);
	mNameTable.UnRegister(object->GetName());





	mObjectFactory->ReleaseObject(object);
}

