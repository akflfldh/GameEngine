#include "RegisterEditorObjectHelper.h"


#include"ObjectFactory/ObjectFactory.h"
#include"ObjectFactory/RuntimeObjectFactory.h"

Quad::RegisterEditorObjectHelper::RegisterEditorObjectHelper(const std::string& className, const std::function<Object* ()>& constructor, const std::function<void(Object*)>& destructor)
{
	EditorObjectTable::RegisterObjectClassName(className);

	EditObjectFactory::GetInstance()->RegisterObjectCtorDtor(className, constructor, destructor);
	RuntimeObjectFactory::GetInstance()->RegisterObjectCtorDtor(className, constructor, destructor);


}


const std::vector<std::string>& Quad::EditorObjectTable::GetObjectTable()
{
	static std::vector<std::string> objectTable;
	return objectTable;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::EditorObjectTable::RegisterObjectClassName(const std::string& name)
{
	std::vector<std::string>& objectTable = const_cast<std::vector<std::string>&>(GetObjectTable());
	objectTable.push_back(name);
}
