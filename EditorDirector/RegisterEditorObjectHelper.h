#pragma once

#include"Predeclare.h"
#include<functional>
#include<string>
#include<vector>
namespace Quad
{
	class RegisterEditorObjectHelper
	{
	public:
		RegisterEditorObjectHelper(const std::string& className, const std::function<Object* ()>& constructor, const std::function<void(Object*)>& destructor);
	private:

	};


	class EditorObjectTable
	{
	public:
		static const std::vector<std::string>& GetObjectTable();
		static void RegisterObjectClassName(const std::string& name);
		//static RemoveObjectClassName(const std::string& name);

	private:


	};




}
