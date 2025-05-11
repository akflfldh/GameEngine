#pragma once


#include<unordered_map>
#include<string>
#include<functional>
#include"Utility/UniqueIDTable.h"
#include"Utility/NameTable.h"

#include"ObjectFactory/BaseObjectFactory.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	class Object;

	class CORE_API_LIB RuntimeObjectFactory:public BaseObjectFactory
	{

		friend class RuntimeObjectManager;
	public:
	
		~RuntimeObjectFactory();
		
		static RuntimeObjectFactory* GetInstance() ;


		virtual Object* CreateObject(const std::string& className) override;



		virtual void RegisterObjectCtorDtor(const std::string& objectClassName, const std::function<Object* ()>& constructor,const std::function<void(Object*)> & destructor);


		virtual void ReleaseObject(Object* object)override;

	

	private:
		RuntimeObjectFactory();

		std::unordered_map<std::string, std::function<Object* ()>> mConstructorTable;
		std::unordered_map<std::string, std::function<void (Object *)>>mDestructorTable;

		UniqueIDTable<Object> mUniqueIDTable;
		NameTable mNameTable;
	};

}
