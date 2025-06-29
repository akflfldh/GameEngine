#pragma once



#include"ObjectManager/BaseObjectManager.h"


namespace Quad
{

	class EditorStaticUserObjectManager:public BaseObjectManager
	{
	public:
		static EditorStaticUserObjectManager* GetInstance();
		EditorStaticUserObjectManager();
		virtual ~EditorStaticUserObjectManager();

		virtual Object* CreateObject(const std::string& className, const std::string& name) override;
		virtual Object* CreateObjectFromFile(const std::string& className, const std::string& name, unsigned long long id) override;

	private:

		void RegisterEditorComponentToObject(Object* object);

	};

}
