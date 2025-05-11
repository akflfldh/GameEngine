#include "ObjectManager/EditGameObjectManager.h"

#include"ObjectFactory/ObjectFactory.h"
#include"Utility/NameTable.h"
#include"Utility/UniqueIDTable.h"
#include"Object/Object.h"
#include"Map/Map.h"


Quad::EditGameObjectManager::EditGameObjectManager()
	:BaseObjectManager(EditObjectFactory::GetInstance())
{



}



//Quad::Object* Quad::EditGameObjectManager::CreateObject(const std::string& className, const std::string& name)
//{
//
//	if (mNameTable.GetID(name) != 0)
//		return nullptr;
//
//
//	Object* object = mObjectFactory->CreateObject(className);
//
//	mUniqueIDTable.Register(object);
//	mNameTable.Register(name, object->GetUniqueID());
//
//	//object->SetName(name);
//
//	return object;
//
//}
//
//
//Quad::Object* Quad::EditGameObjectManager::CreateObjectFromFile(const std::string& className, const std::string& name, unsigned long long id)
//{
//
//	if (mNameTable.GetID(name) != 0 || mUniqueIDTable.GetElement(id) != nullptr)
//		return nullptr;
//
//
//	Object* object = mObjectFactory->CreateObject(className);
//
//	mUniqueIDTable.RegisterFromFile(object, id);
//	mNameTable.Register(name, id);
//	return object;
//}
//
//
//void Quad::EditGameObjectManager::KillObject(Object* object)
//{
//	mDeadObjectTable.push_back(object);
//}
//
//void Quad::EditGameObjectManager::RemoveDeadObject()
//{
//	for (auto element : mDeadObjectTable)
//	{
//		Map* map = element->GetMap();
//		map->RemoveObject(element);
//
//		ReleaseObject(element);
//	}
//	mDeadObjectTable.resize(0);
//}
//
//Quad::Object* Quad::EditGameObjectManager::GetObjectFromID(unsigned long long id)
//{
//
//	auto instance = GetInstance();
//	if (id == 0)
//		return nullptr;
//
//	return instance->mUniqueIDTable.GetElement(id);
//
//}
//
//Quad::Object* Quad::EditGameObjectManager::GetObjectFromName(const std::string& name)
//{
//	auto instance = GetInstance();
//
//	unsigned long long id = instance->mNameTable.GetID(name);
//
//
//	return GetObjectFromID(id);
//
//}
//
//bool Quad::EditGameObjectManager::ChangeObjectName(unsigned long long id, const std::string& beforeName, const std::string& newName)
//{
//	if (id == 0)
//		return false;
//
//	auto instance = GetInstance();
//	instance->mNameTable.UnRegister(beforeName);
//	return instance->mNameTable.Register(newName, id);
//}
//
//unsigned long long Quad::EditGameObjectManager::PeekObjectUniqueID() const
//{
//	return mUniqueIDTable.PeekUniqueID();
//}
//
//void Quad::EditGameObjectManager::SetNextAvailableObjectUniqueID(unsigned long long id)
//{
//
//	mUniqueIDTable.SetNextAvailalbeUniqueID(id);
//
//}
//
//void Quad::EditGameObjectManager::ReleaseObject(Object* object)
//{
//	mUniqueIDTable.UnRegister(object);
//	mNameTable.UnRegister(object->GetName());
//	mObjectFactory->ReleaseObject(object);
//}
//
