#pragma once

#include<string>
#include<vector>
#include"Utility/Singleton.h"
#include"ObjectManager/BaseObjectManager.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	class Object; 
	class RuntimeObjectFactory;


	class CORE_API_LIB RuntimeObjectManager:public Singleton<RuntimeObjectManager>,public BaseObjectManager
	{

	public:
		RuntimeObjectManager();
		~RuntimeObjectManager() = default;

		//virtual Object* CreateObject(const std::string& className, const std::string& name) override;
		//virtual Object* CreateObjectFromFile(const std::string& className, const std::string& name, unsigned long long id) override;


		//virtual void KillObject(Object* object);
		//virtual void RemoveDeadObject();

		//virtual Object* GetObjectFromID(unsigned long long id);
		//virtual Object* GetObjectFromName(const std::string& name);
		//virtual bool ChangeObjectName(unsigned long long id, const std::string& beforeName, const std::string& newName) override;


		//virtual void ReleaseObject(Object* object) override;
	private:


		//RuntimeObjectFactory* mObjectFactory;
		//UniqueIDTable<Object> mUniqueIDTable;
		//NameTable mNameTable;

		//std::vector<Object*> mDeadObjectTable;

	};

}