#pragma once


#include"Core/CoreDllExport.h"
#include<string>
namespace Quad
{
	class CORE_API_LIB ProjectConfig
	{
	public:
		static ProjectConfig* GetInstance();
		~ProjectConfig();


		void SetProjectPath(const std::string & path);
		const std::string& GetProjectPath() const;


	private:
		std::string mProjectPath;
		




	private:
		ProjectConfig();


	};

}
