#pragma once


#include"Predeclare.h"

#include<unordered_map>
#include<string>
#include<functional>

#include"CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB ComponentFactory
	{
	public:
		~ComponentFactory();
		static ComponentFactory* GetInstance();


		static BaseComponent* CreateComponent(const std::string& componentName);
		static void ReleaseComponent(BaseComponent* component);


		static void RegisterComponentCtorDtor(const std::string& componentName,const std::function<BaseComponent* ()> & ctor, const std::function<void(BaseComponent*)> & dtor);



	private:
		ComponentFactory();

	private:


		std::unordered_map<std::string, std::function<BaseComponent* ()>> mConstructorTable;
		std::unordered_map<std::string, std::function<void(BaseComponent*)>> mDestructorTable;

	};

}
