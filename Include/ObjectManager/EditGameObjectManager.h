#pragma once


#include"Predeclare.h"
#include"Utility/Singleton.h"
#include"ObjectManager/BaseObjectManager.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB EditGameObjectManager:public BaseObjectManager, public Singleton<EditGameObjectManager>
	{
	public:
		EditGameObjectManager();
		~EditGameObjectManager() = default;

	/*	virtual Object* CreateObject(const std::string& className, const std::string& name) override;
		virtual Object* CreateObjectFromFile(const std::string& className, const std::string& name, unsigned long long id) override;

		virtual void KillObject(Object* object)override;
		virtual void RemoveDeadObject()override;

		virtual Object* GetObjectFromID(unsigned long long id) override;
		virtual Object* GetObjectFromName(const std::string& name)override;
		virtual bool ChangeObjectName(unsigned long long id, const std::string& beforeName, const std::string& newName) override;


		unsigned long long PeekObjectUniqueID() const;


		void SetNextAvailableObjectUniqueID(unsigned long long id);


		virtual void ReleaseObject(Object* object) override;*/
	private:

		/*EditObjectFactory* mObjectFactory;
		UniqueIDTable<Object> mUniqueIDTable;
		NameTable mNameTable;

		std::vector<Object*> mDeadObjectTable;*/
	};

}