#pragma once

#include<unordered_map>
#include<string>


#ifdef UTILITY_LIB_EXPORT
#define UTILITY_LIB_API __declspec(dllexport)
#else
#define UTILITY_LIB_API __declspec(dllimport)
#endif

namespace Quad
{
	//name - id 
	class UTILITY_LIB_API NameTable
	{
	public:
		NameTable() = default;
		~NameTable() = default;


		bool Register(const std::string& key, unsigned long long id);
		void UnRegister(const std::string& key);

	


		unsigned long long GetID(const std::string& key) const;

		int GetElementNum() const;
	private:
		
		std::unordered_map<std::string, unsigned long long> mTable;
		
	};


}