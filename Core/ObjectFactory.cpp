#include "ObjectFactory/ObjectFactory.h"
#include"Object/Object.h"

Quad::EditObjectFactory::EditObjectFactory()
{


}


Quad::EditObjectFactory::~EditObjectFactory()
{
}

Quad::EditObjectFactory* Quad::EditObjectFactory::GetInstance()
{
	static EditObjectFactory objectFactory;
	return &objectFactory;
}

Quad::Object* Quad::EditObjectFactory::CreateObject(const std::string& className)
{
	auto instance = GetInstance();
	std::function<Object* ()>& func = instance->mConstructorTable[className];
	
	Object* object = func();

	return object;

}

//Quad::Object* Quad::EditObjectFactory::CreateObjectFromFile(const std::string& className, const std::string& name, unsigned long long id)
//{
//
//	auto instance = GetInstance();
//
//	if (instance->mNameTable.GetID(name) != 0)
//		return nullptr;
//
//
//	Object * object=  instance->mConstructorTable[className]();
//	object->SetName(name);
//	instance->mUniqueIDTable.RegisterFromFile(object, id);
//	instance->mNameTable.Register(name, id);
//
//	return object;
//}

void Quad::EditObjectFactory::RegisterObjectCtorDtor(const std::string& objectClassName, const std::function<Object* ()>& constructor, const std::function<void(Object*)>& destructor)
{
	auto instance = GetInstance();
	instance->mConstructorTable[objectClassName] = constructor;
	instance->mDestructorTable[objectClassName] = destructor;
}

void Quad::EditObjectFactory::ReleaseObject(Object* object)
{
	auto instance = GetInstance();
	instance->mDestructorTable[object->GetClassTypeName()](object);
}

const std::unordered_map<std::string, std::function<Quad::Object* ()>>& Quad::EditObjectFactory::GetObjectConstructorTable() const
{
	return mConstructorTable;
	// TODO: 여기에 return 문을 삽입합니다.
}

