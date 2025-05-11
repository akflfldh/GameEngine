#include "ObjectFactory/RuntimeObjectFactory.h"

//#include "ObjectFactory.h"
#include"Object/Object.h"

Quad::RuntimeObjectFactory::RuntimeObjectFactory()
{


}


Quad::RuntimeObjectFactory::~RuntimeObjectFactory()
{
}

Quad::RuntimeObjectFactory* Quad::RuntimeObjectFactory::GetInstance()
{
	static RuntimeObjectFactory objectFactory;
	return &objectFactory;
}

Quad::Object* Quad::RuntimeObjectFactory::CreateObject(const std::string& className)
{
	auto instance = GetInstance();


	Object* object = instance->mConstructorTable[className]();
	return object;

}



void Quad::RuntimeObjectFactory::RegisterObjectCtorDtor(const std::string& objectClassName, const std::function<Object* ()>& constructor, const std::function<void(Object*)>& destructor)
{
	auto instance = GetInstance();
	instance->mConstructorTable[objectClassName] = constructor;
	instance->mDestructorTable[objectClassName] = destructor;
}

void Quad::RuntimeObjectFactory::ReleaseObject(Object* object)
{

	auto instance = GetInstance();
	instance->mDestructorTable[object->GetClassTypeName()](object);
}


