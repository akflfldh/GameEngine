#pragma once

#include<string>


#include"Utility/UniqueIDTable.h"
#include"Utility/NameTable.h"

#include"Core/CoreDllExport.h"

namespace Quad
{
	class Object;
	class BaseObjectFactory;
	
	class CORE_API_LIB BaseObjectManager
	{
	public:
		BaseObjectManager(BaseObjectFactory* objectFactory);
		virtual ~BaseObjectManager();

		virtual Object* CreateObject(const std::string& className, const std::string& name) ;
		virtual Object* CreateObjectFromFile(const std::string& className, const std::string& name, unsigned long long id) ;

		virtual void KillObject(Object* object) ;

		//map removeObject호출후, ReleaseObject호출 수행
		virtual void RemoveDeadObject() ;

		virtual Object* GetObjectFromID(unsigned long long id);
		virtual Object* GetObjectFromName(const std::string& name);
		virtual bool ChangeObjectName(unsigned long long id, const std::string& beforeName, const std::string& newName) ;


		//object를 id,name table에서 해제하고, factory에 release까지 수행하는메서드 
		//RemoveDeadObject내부에서 호출한다.
		//Map에서 Reset할때 효율성을 위해 이메서드를 직접호출
		virtual void ReleaseObject(Object * object) ;

		unsigned long long PeekObjectUniqueID() const;
		void SetNextAvailableObjectUniqueID(unsigned long long id);


		int GetNameTableElementNum()const;
	protected:

		BaseObjectFactory* GetObjectFactory()const;

		/*const UniqueIDTable<Object>& GetUniqueIDTable() const;
		UniqueIDTable<Object>& GetUniqueIDTable();
		const NameTable& GetNameTable() const;
		NameTable& GetNameTable();

		const std::vector<Object*>& GetDeadObjectTable() const;
		std::vector<Object*>& GetDeadObjectTable();*/

		

	private:


		BaseObjectFactory* mObjectFactory;
		UniqueIDTable<Object> mUniqueIDTable;
		NameTable mNameTable;

		std::vector<Object*> mDeadObjectTable;



	};

}
