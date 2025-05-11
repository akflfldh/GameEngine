#include "ObjectManager/RuntimeGameObjectManager.h"
#include"ObjectFactory/ObjectFactory.h"

#include"Utility/NameTable.h"
#include"Utility/UniqueIDTable.h"
#include"Object/Object.h"
#include"Map/Map.h"

Quad::RuntimeGameObjectManager::RuntimeGameObjectManager()
    :BaseObjectManager(EditObjectFactory::GetInstance())
{
}

//Quad::Object* Quad::RuntimeGameObjectManager::CreateObject(const std::string& className, const std::string& name)
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
////	object->SetName(name);
//
//	return object;
//
//}
//
//
//Quad::Object* Quad::RuntimeGameObjectManager::CreateObjectFromFile(const std::string& className, const std::string& name, unsigned long long id)
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
//void Quad::RuntimeGameObjectManager::KillObject(Object* object)
//{
//	mDeadObjectTable.push_back(object);
//}
//
//void Quad::RuntimeGameObjectManager::RemoveDeadObject()
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
//Quad::Object* Quad::RuntimeGameObjectManager::GetObjectFromID(unsigned long long id)
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
//Quad::Object* Quad::RuntimeGameObjectManager::GetObjectFromName(const std::string& name)
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
//bool Quad::RuntimeGameObjectManager::ChangeObjectName(unsigned long long id, const std::string& beforeName, const std::string& newName)
//{
//	if (id == 0)
//		return false;
//
//	auto instance = GetInstance();
//	instance->mNameTable.UnRegister(beforeName);
//	return instance->mNameTable.Register(newName, id);
//}
//
//void Quad::RuntimeGameObjectManager::ReleaseObject(Object* object)
//{
//
//
//	mUniqueIDTable.UnRegister(object);
//	mNameTable.UnRegister(object->GetName());
//	mObjectFactory->ReleaseObject(object);
//}
