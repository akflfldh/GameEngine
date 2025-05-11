#include "ObjectManager/ObjectManager.h"
#include"ObjectFactory/ObjectFactory.h"
#include"Object/Object.h"
#include"Core/ObjectBinnedAllocator.h"
#include"Map/Map.h"

#include"ObjectFactory/ObjectFactory.h"

Quad::EditObjectManager::EditObjectManager()
	:BaseObjectManager(EditObjectFactory::GetInstance())
{


}
const std::unordered_map<std::string, std::function<Quad::Object* ()>>& Quad::EditObjectManager::GetObjectConstructTable() const
{

	EditObjectFactory* objectFactory = static_cast<EditObjectFactory*>(GetObjectFactory());
	return objectFactory->GetObjectConstructorTable();

	// TODO: 여기에 return 문을 삽입합니다.
}
//
//Quad::Object* Quad::EditObjectManager::CreateObject(const std::string& className, const std::string& name)
//{
//
//	if(mNameTable.GetID(name) != 0)
//		return nullptr;
//		
//
//	Object * object=  mObjectFactory->CreateObject(className);
//	
//	mUniqueIDTable.Register(object);
//	mNameTable.Register(name, object->GetUniqueID());
//	//object->SetName(name);
//	return object;
//
//}
//
//
//Quad::Object* Quad::EditObjectManager::CreateObjectFromFile(const std::string& className, const std::string& name, unsigned long long id)
//{
//
//	if (mNameTable.GetID(name) != 0  ||  mUniqueIDTable.GetElement(id) !=nullptr)
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
//void Quad::EditObjectManager::KillObject(Object* object)
//{
//	mDeadObjectTable.push_back(object);
//}
//
//void Quad::EditObjectManager::RemoveDeadObject()
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
//Quad::Object* Quad::EditObjectManager::GetObjectFromID(unsigned long long id)
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
//Quad::Object* Quad::EditObjectManager::GetObjectFromName(const std::string& name)
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
//bool Quad::EditObjectManager::ChangeObjectName(unsigned long long id, const std::string& beforeName, const std::string& newName)
//{
//	return false;
//}
//
//void Quad::EditObjectManager::ReleaseObject(Object* object)
//{
//	mUniqueIDTable.UnRegister(object);
//	mNameTable.UnRegister(object->GetName());
//	mObjectFactory->ReleaseObject(object);
//
//}
