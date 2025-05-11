#pragma once

#include"Predeclare.h"
#include"Core/ContentItem.h"
#include<string>

namespace Quad
{
	class ProjectMetaData : public ContentItem
	{	
	public:
		ProjectMetaData(const std::string& projectName="");
		virtual ~ProjectMetaData();

		void SetProjectFolderPath(const std::string& folderPath);
		const std::string& GetProjectFolderPath()const;

		virtual void Serialize(const std::string& tag = "") override;
		virtual void DeSerialize(const std::string& tag = "")override;

	private:
		std::string mProjectFolderPath;



	};
}
