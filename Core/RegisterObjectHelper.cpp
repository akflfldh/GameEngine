#include "Object/RegisterObjectHelper.h"

#include"ObjectFactory/ObjectFactory.h"
#include"ObjectFactory/RuntimeObjectFactory.h"
Quad::RegisterObjectHelper::RegisterObjectHelper(const std::string& className, const std::function<Object* ()>& constructor, const std::function<void(Object*)>& destructor)
{
	bool ret =ObjectTable::RegisterObjectClassName(className);
	if (ret)
	{
		EditObjectFactory::GetInstance()->RegisterObjectCtorDtor(className, constructor, destructor);
		RuntimeObjectFactory::GetInstance()->RegisterObjectCtorDtor(className, constructor, destructor);
	}
}



const std::vector<std::string> & Quad::ObjectTable::GetObjectTable()
{
	static std::vector<std::string> objectTable;
	return objectTable;
	// TODO: 여기에 return 문을 삽입합니다.
}

bool Quad::ObjectTable::RegisterObjectClassName(const std::string& name)
{

	std::vector<std::string> & objectTable =const_cast<std::vector<std::string>&>(GetObjectTable());

	auto it = std::find(objectTable.begin(), objectTable.end(), name);
	if (it == objectTable.end())
	{
		
		objectTable.push_back(name);
		return true;
	}

	return false;
}
