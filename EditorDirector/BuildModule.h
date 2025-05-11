#pragma once


#include"Utility/Singleton.h"
#include<string>
namespace Quad
{
	class BuildModule:public Singleton<BuildModule>
	{
	public:
		BuildModule();
		~BuildModule();


		static bool Build(const std::string& outputBuildPath, const std::string& programName);


	private:
		static bool GenerateFinalAssetFile(const std::string& outputAssetPath, const std::string& currProjectPath);
		static bool CopyProjectFiles(const std::string& programFolderPath, const std::string& programName);
	};
}