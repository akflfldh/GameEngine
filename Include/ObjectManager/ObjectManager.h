#pragma once

#include"Utility/Singleton.h"
#include<vector>
#include"ObjectManager/BaseObjectManager.h"

#include"Core/CoreDllExport.h"
#include<string>
#include<unordered_map>
#include<functional>
namespace Quad
{
	class Object;
	class EditObjectFactory;
	

	class CORE_API_LIB EditObjectManager :public Singleton<EditObjectManager>, public BaseObjectManager
	{
	
		 

	public:
		EditObjectManager();
		~EditObjectManager() = default;

	/*	virtual Object* CreateObject(const std::string& className, const std::string& name) override;
		virtual Object* CreateObjectFromFile(const std::string& className, const std::string& name, unsigned long long id) override;

		virtual void KillObject(Object* object);
		virtual void RemoveDeadObject();

		virtual Object* GetObjectFromID(unsigned long long id);
		virtual Object* GetObjectFromName(const std::string& name);
	
		virtual bool ChangeObjectName(unsigned long long id, const std::string& beforeName, const std::string& newName) override;


		virtual void ReleaseObject(Object* object) override;*/


		const std::unordered_map<std::string, std::function<Object* ()>>& GetObjectConstructTable() const;

	private:
	

		//EditObjectFactory* mObjectFactory;
		//UniqueIDTable<Object> mUniqueIDTable;
		//NameTable mNameTable;

		//std::vector<Object*> mDeadObjectTable;

	};


}
