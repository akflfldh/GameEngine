#pragma once



#include"Predeclare.h"
#include<unordered_map>
#include<functional>
#include"Utility/Singleton.h"
#include<string>
#include"Utility/UniqueIDTable.h"
#include"Utility/NameTable.h"
#include"ObjectFactory/BaseObjectFactory.h"

#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB EditObjectFactory :public BaseObjectFactory
	{
		friend class EditObjectManager;

	public:
	
		~EditObjectFactory();

		static EditObjectFactory* GetInstance();

		virtual Object* CreateObject(const std::string& className) override;
		//virtual Object* CreateObjectFromFile(const std::string & className, const std::string& name, unsigned long long id);
		virtual void RegisterObjectCtorDtor(const std::string& objectClassName, const std::function<Object* ()>& constructor, const std::function<void(Object*)>& destructor)override;

		virtual void ReleaseObject(Object* object) override;
		
		const std::unordered_map<std::string, std::function<Object* ()>>& GetObjectConstructorTable() const;

	private:
		EditObjectFactory();
		std::unordered_map<std::string, std::function<Object* ()>> mConstructorTable;
		std::unordered_map<std::string, std::function<void (Object *)>> mDestructorTable;






		//UniqueIDTable<Object> mUniqueIDTable;
		//NameTable mNameTable;

	};


}
