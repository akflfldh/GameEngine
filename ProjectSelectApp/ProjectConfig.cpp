#include "ProjectConfig.h"




QuadPSA::ProjectConfig::ProjectConfig()
{

}

QuadPSA::ProjectConfig::~ProjectConfig()
{


}

void QuadPSA::ProjectConfig::Initialize(const std::string& path, const std::string& version, const std::string& name)
{
	SetPath(path);
	SetVersion(version);
	SetName(name);

}




void QuadPSA::ProjectConfig::SetPath(const std::string& path)
{

	mPath = path;
}

const std::string& QuadPSA::ProjectConfig::GetPath() const
{

	return mPath;
	// TODO: 여기에 return 문을 삽입합니다.
}

void QuadPSA::ProjectConfig::SetVersion(const std::string& version)
{

	mVersion = version;
}

const std::string& QuadPSA::ProjectConfig::GetVersion() const
{
	return mVersion;

	// TODO: 여기에 return 문을 삽입합니다.
}

void QuadPSA::ProjectConfig::SetName(const std::string& name)
{
	mName = name;
}
const std::string& QuadPSA::ProjectConfig::GetName() const
{
	return mName;
}