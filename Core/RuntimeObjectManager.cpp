
#include "ObjectManager/RuntimeObjectManager.h"
#include"ObjectFactory/RuntimeObjectFactory.h"
#include"Object/Object.h"
#include"Allocator/BinnedAllocator.h"
#include"Core/ObjectBinnedAllocator.h"
#include"Map/Map.h"

Quad::RuntimeObjectManager::RuntimeObjectManager()
	:BaseObjectManager(RuntimeObjectFactory::GetInstance())
{


}

//Quad::Object* Quad::RuntimeObjectManager::CreateObject(const std::string& className, const std::string& name)
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
//Quad::Object* Quad::RuntimeObjectManager::CreateObjectFromFile(const std::string& className, const std::string& name, unsigned long long id)
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
//void Quad::RuntimeObjectManager::KillObject(Object* object)
//{
//	mDeadObjectTable.push_back(object);
//}
//
//void Quad::RuntimeObjectManager::RemoveDeadObject()
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
//Quad::Object* Quad::RuntimeObjectManager::GetObjectFromID(unsigned long long id)
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
//Quad::Object* Quad::RuntimeObjectManager::GetObjectFromName(const std::string& name)
//{
//	auto instance = GetInstance();
//
//	unsigned long long id = instance->mNameTable.GetID(name);
//	return GetObjectFromID(id);
//
//}
//
//bool Quad::RuntimeObjectManager::ChangeObjectName(unsigned long long id, const std::string& beforeName, const std::string& newName)
//{
//	if (id == 0)
//		return false;
//
//	auto instance = GetInstance();
//	instance->mNameTable.UnRegister(beforeName);
//	return instance->mNameTable.Register(newName, id);
//}
//
//void Quad::RuntimeObjectManager::ReleaseObject(Object* object)
//{
//
//	mUniqueIDTable.UnRegister(object);
//	mNameTable.UnRegister(object->GetName());
//	mObjectFactory->ReleaseObject(object);
//
//
//}
