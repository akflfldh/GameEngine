#pragma once


#include<string>
#include<functional>

#include"Core/CoreDllExport.h"
#include<vector>

namespace Quad
{
	class Object;

	class CORE_API_LIB RegisterObjectHelper
	{
	public:
		RegisterObjectHelper(const std::string& className, const std::function<Quad::Object* ()>& constructor, const std::function<void(Quad::Object*)>& destructor);
		~RegisterObjectHelper() = default;

	private:

	};


	//유저의 오브젝트 클래스 테이블
	class CORE_API_LIB ObjectTable
	{
	public:
		static const std::vector<std::string> &  GetObjectTable();
		static bool RegisterObjectClassName(const std::string& name);

	private:
	
	};

}