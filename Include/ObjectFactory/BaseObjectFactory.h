#pragma once

#include<string>
#include<functional>


#include"Core/CoreDllExport.h"
namespace Quad
{
	class Object;
	class CORE_API_LIB  BaseObjectFactory
	{
	public:

		virtual Object* CreateObject(const std::string& className) =0;
		//virtual Object* CreateObjectFromFile(const std::string& className) = 0;
		virtual void RegisterObjectCtorDtor(const std::string& objectClassName, const std::function<Object* ()>& constructor, const std::function<void(Object*)>& destructor) = 0;

		virtual void ReleaseObject(Object* object)= 0;

		

	private:




	};


}
