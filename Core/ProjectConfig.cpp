#include "ProjectConfig.h"

Quad::ProjectConfig* Quad::ProjectConfig::GetInstance()
{
	static ProjectConfig instance;

	return &instance;


}

Quad::ProjectConfig::ProjectConfig()
{
	
}

Quad::ProjectConfig::~ProjectConfig()
{


}



void Quad::ProjectConfig::SetProjectPath(const std::string& path)
{
	mProjectPath = path;
}

const std::string& Quad::ProjectConfig::GetProjectPath() const
{
	return mProjectPath;
	// TODO: 여기에 return 문을 삽입합니다.
}
