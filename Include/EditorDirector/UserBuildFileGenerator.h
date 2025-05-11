#pragma once

#include<string>


namespace Quad
{
	
	class UserBuildFileGenerator
	{
	public:
		UserBuildFileGenerator() = delete;
		~UserBuildFileGenerator() = default;

		static bool CreateBuildFile(const std::string& templateDirectoryPath, const std::string userProjectDirectory);
		static bool CreateClassFile(const std::string& className, const std::string& templateDirectoryPath, const std::string userProjectDirectory);


	private:
		static bool CreateProjectFile(const std::string& UserProjectDirectory, const std::string& name ,const std::string & projectData);
		static bool CreateSolutionFile(const std::string& UserProjectDirectory, const std::string& name ,const std::string& solutionData);
		static bool CreateFilterFile(const std::string& UserProjectDirectory, const std::string& name ,const std::string& filterData);

	};


};