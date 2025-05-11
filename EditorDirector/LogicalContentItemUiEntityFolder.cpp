#include "LogicalContentItemUiEntityFolder.h"
#include"PanelUiEntity.h"
#include"FileUiSystemCamera.h"
#include"System/FileUiUiSystem.h"
#include"PanelUiEntity.h"
#include"ScrollBarEntity.h"
#include"ContentItemUiPanelEntity.h"


Quad::UniqueIDTable<Quad::LogicalContentItemUiEntityFolder> Quad::LogicalContentItemUiEntityFolder::mUniqueIDTable;

Quad::LogicalContentItemUiEntityFolder::LogicalContentItemUiEntityFolder(const std::string& name, FileUiUiSystem* system, FolderPanelEntity* folderPanelEntity )
	:ContentItem(name, EContentItemType::eLogicalFolder), mContentItemUiPanelEntity(nullptr), mLinkedFolderPanelEntity(folderPanelEntity)
	, mUniqueID(0)
{

	mUniqueIDTable.Register(this);

	//mScrollBarEntity = new ScrollBarEntity(name+L"_ScrollBar");
	//mScrollBarEntity->SetSystem(system);
	//mScrollBarEntity->Initialize(system->GetMap()->GetGameCamera(),mPanelUiEntity);
	//system->AddEntity(mScrollBarEntity);

	static float colorIntensity = 0;
	Map* map = system->GetMap();
	int mapLayerIndex = 0;

	float clientWidth = system->GetClientWidth();
	float clientHeight = system->GetClientHeight();

	//mContentItemUiPanelEntity = new ContentItemUiPanelEntity(name + "_ContentItemUiPanelEntity");
	//mContentItemUiPanelEntity = static_cast<ContentItemUiPanelEntity*>(map->CreateObject("ContentItemUiPanelEntity"));


	mContentItemUiPanelEntity = ContentItemUiPanelEntity::Create(map, mapLayerIndex, 3000, 50000, 250, mContentItemUiPanelOffsetY, 10, clientWidth, clientHeight);

	//mContentItemUiPanelEntity = new ContentItemUiPanelEntity(name + "_ContentItemUiPanelEntity");
//	mContentItemUiPanelEntity->SetSystem(system);
	

	//mContentItemUiPanelEntity->Initialize(3000, 3000, 250, mContentItemUiPanelOffsetY, 10, clientWidth,clientHeight);
	mContentItemUiPanelEntity->SetColorItensity({ colorIntensity,colorIntensity,colorIntensity });
	colorIntensity += 0.1F;


	mContentItemUiPanelEntity->SetActiveFlag(false);
	mContentItemUiPanelEntity->SetDrawFlag(false);
	mContentItemUiPanelEntity->SetSelectAvailableFlag(false);


//	system->AddEntity(mContentItemUiPanelEntity);








}


Quad::LogicalContentItemUiEntityFolder::~LogicalContentItemUiEntityFolder()
{
	mUniqueIDTable.UnRegister(GetUniqueID());
}

void Quad::LogicalContentItemUiEntityFolder::AddContentItemUiEntity(ContentItemUiEntity* contentItemUiEntity)
{
	mContentItemUiPanelEntity->AddContentItemUiEntity(contentItemUiEntity);



}


int Quad::LogicalContentItemUiEntityFolder::GetContentItemNum() const
{
	return mContentItemNum;
}

//const std::vector<Quad::ContentItemUiEntity*>& Quad::LogicalContentItemUiEntityFolder::GetContentItemUiEntityVector() const
//{
//
//	return mContentItemUiEntityVector;
//	// TODO: 여기에 return 문을 삽입합니다.
//}

Quad::ContentItemUiPanelEntity* Quad::LogicalContentItemUiEntityFolder::GetContentItemPanelEntity() const
{
	return mContentItemUiPanelEntity;
}



Quad::ScrollBarEntity* Quad::LogicalContentItemUiEntityFolder::GetScrollBarEntity() const
{
	return mScrollBarEntity;
}


Quad::FolderPanelEntity* Quad::LogicalContentItemUiEntityFolder::GetLinkedFolderPanelEntity() const
{
	return mLinkedFolderPanelEntity;
}

void Quad::LogicalContentItemUiEntityFolder::ActivateContentItemUiPanelEntity(bool flag)
{
	mContentItemUiPanelEntity->ActivatePanel(flag);


}

void Quad::LogicalContentItemUiEntityFolder::SetUniqueID(unsigned long long id)
{
	mUniqueID = id;
}

unsigned long long Quad::LogicalContentItemUiEntityFolder::GetUniqueID() const
{
	return mUniqueID;
}

