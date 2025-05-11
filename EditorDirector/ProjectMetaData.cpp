#include "EditorDirector/ProjectMetaData.h"
#include"Parser/JsonParser.h"
Quad::ProjectMetaData::ProjectMetaData(const std::string& name)
	:ContentItem(name,EContentItemType::eProject)
{
}

Quad::ProjectMetaData::~ProjectMetaData()
{

}

void Quad::ProjectMetaData::SetProjectFolderPath(const std::string& folderPath)
{
	mProjectFolderPath = folderPath;

}

const std::string& Quad::ProjectMetaData::GetProjectFolderPath() const
{
	return mProjectFolderPath;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::ProjectMetaData::Serialize(const std::string& tag)
{
	ContentItem::Serialize(tag);
	JsonParser::Write("ProjectMetaData_ProjectFolerPath_" + tag, mProjectFolderPath);
	
}


void Quad::ProjectMetaData::DeSerialize(const std::string& tag)
{
	ContentItem::DeSerialize(tag);
	JsonParser::Read("ProjectMetaData_ProjectFolerPath_", mProjectFolderPath);

}
