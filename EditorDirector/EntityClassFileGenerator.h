#pragma once
#include<string>

namespace Quad
{
	class EntityClassFileGenerator
	{
	public:
		EntityClassFileGenerator() = delete;
		~EntityClassFileGenerator() = default;


		static bool GenerateFile(const std::string& className, const std::string& templateDirectoryPath, const std::string userProjectDirectory);


	private:

	};

}
