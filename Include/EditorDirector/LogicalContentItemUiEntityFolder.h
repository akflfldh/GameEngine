#pragma once




#include"Predeclare.h"
#include"ContentItem.h"
#include"ContentItemUiEntity.h"

#include<vector>

#include"UniqueIDTable.h"
namespace Quad
{


	//map을 가진다.
	//당연히 fileUiSystem에서 사용되는것이기에
	//카메라설정,등은 그시스템의설정을 따른다.
	//폴더생성시 자체 내부에서 map을 생성한다. 
	//폴더도 이름이있을것.
	//그 폴더를 assetUiEntity,그리고 옆에 리스트에 막대로도 나타낼것이다.

	//폴더안에 asset들과 다른 자식폴더들을 가질것이다(모두 contentItem)
	
	class LogicalContentItemUiEntityFolder:public ContentItem
	{
	public:
		LogicalContentItemUiEntityFolder(const std::string & name, FileUiUiSystem* system,
			FolderPanelEntity* folderPanelEntity);
		virtual ~LogicalContentItemUiEntityFolder();

		void AddContentItemUiEntity(ContentItemUiEntity* contentItem);

		int GetContentItemNum() const;
		//const std::vector<ContentItemUiEntity*>& GetContentItemUiEntityVector() const;

		ContentItemUiPanelEntity* GetContentItemPanelEntity() const;


		ScrollBarEntity* GetScrollBarEntity()const;

		//void SetOnOff(bool flag);

		FolderPanelEntity* GetLinkedFolderPanelEntity() const;

		void ActivateContentItemUiPanelEntity(bool flag);

		void SetUniqueID(unsigned long long id);
		unsigned long long GetUniqueID() const;
	

	private:
		ContentItemUiPanelEntity* mContentItemUiPanelEntity;
		ScrollBarEntity* mScrollBarEntity;
		int mContentItemNum;


		//map안에있는 씬그래프에서 빠른조회를위해 unordered_map이있지만
		//정렬된상태유지를원하기때문에
		//자체적으로 vector컨테이너를 추가적으로 사용해보자
	

		FolderPanelEntity* mLinkedFolderPanelEntity;

		float mContentItemUiPanelOffsetY = -40.0f;

		static UniqueIDTable<LogicalContentItemUiEntityFolder> mUniqueIDTable;
		unsigned long long mUniqueID;

	};
}

