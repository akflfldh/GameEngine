#include "System/FileUiUiSystem.h"
#include"Object/Camera/Camera.h"
#include"EditorDirector/FileUiEntity.h"
#include"EditorDirector/DirectoryUiEntity.h"
#include"Core/RenderSystem.h"
#include"EditorDirector/ContentItemUiEntity.h"
#include"EditorDirector/LogicalContentItemUiEntityFolder.h"
#include"Core/PanelUiEntity.h"
#include"EditorDirector/DockingWindowController.h"
#include"EditorDirector/DefaultState.h"
#include"EditorDirector/ScrollBarEntity.h"
//#include"TextPanelEntity.h"
#include"EditorDirector/FolderPanelEntity.h"
#include"Object/Camera/OrthogoanlCamera.h"

#include"EditorDirector/FolderListPanelEntity.h"
#include"EditorDirector/ContentItemUiPanelEntity.h"
#include"EditorDirector/NavigationFolderPanelEntity.h"


namespace Quad
{
	FileUiUiSystem::FileUiUiSystem()
		:System(ESystemType::eMainSystem, ESystemID::eFileUiMainSystem,"FileUiUiSystem"), mFolderListPanelEntity(nullptr)
		,mNavigationFolderPanelEntity(nullptr)
	{

	}
	FileUiUiSystem::~FileUiUiSystem()
	{
		
	}

	void FileUiUiSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
	{
		System::Initialize(clientWidth, clientHeight, map);
		SetSystemState(new DefaultState("Default"));

		OnRuntimeMode();
		//SetPlayModeState(true);
		//create folder함수를 제공하면 좋을듯? 외부에서 요청이올때 
		//폴더이름중복되는지도 확인해야할듯
		//mRootLogicalContentItemUiEntityFolderNode = CreateLogicalContentItemUiEntityFolderNode(L"Asset");
	//	mCurrentLogicalContentItemUiEntityFolderNode = mRootLogicalContentItemUiEntityFolderNode;



		//이밑에있는설정들은 모든 폴더에 다 동일하게 적용되는설정
		mContentItemUiEntityWidthSize = 100.0f;
		mContentItemUiEntityHeightSize = 140.0f;
		mDistanceBetweenContentItemUiEntity = 50.f;
		mStartPositionX = 50.0f;
		mStartPositionY = 50.0F;
		mMaxContentItemUiEntityNumInRow = CaculateMaxEntityNumInRow(3 * clientWidth / 4);

		mContentItemUiEntityPanelWidth = 3000;
		mContentItemUiEntityPanelHeight = 2000.0f;

		Map* currMap = GetMap();
		int mapLayerIndex = 0;
		//mFolderListPanelEntity = static_cast<FolderListPanelEntity*>(currMap->CreateObject("FolderListPanelEntity"));
		mFolderListPanelEntity = FolderListPanelEntity::Create(currMap, mapLayerIndex);
	



	//	mFolderListPanelEntity->SetSystem(this);
		//mFolderListPanelEntity->Initialize();
		mFolderListPanelEntity->SetSize(3000, 3000);
		mFolderListPanelEntity->SetPosition(3000 / 2, -1500 + mFoderFanelStartOffsetY, 20.0f);


		//mNavigationFolderPanelEntity = static_cast<NavigationFolderPanelEntity*>(currMap->CreateObject("NavigationFolderPanelEntity"));
		mNavigationFolderPanelEntity = NavigationFolderPanelEntity::Create(currMap, mapLayerIndex, 3000, 40);
		

	//	mNavigationFolderPanelEntity.SetSystem(this);
		//mNavigationFolderPanelEntity->Initialize(3000,40);
		mNavigationFolderPanelEntity->SetPosition(3000 / 2, -1.0f * 40 / 2, 10.0f);

	//	AddEntity(&mNavigationFolderPanelEntity);


		CreateRootFolder();

	//	AddEntity(&mFolderListPanelEntity);
			


		CreateFolder(mCurrentLogicalFolder,reinterpret_cast<const char*>(u8"메시"));
		CreateFolder(mCurrentLogicalFolder, reinterpret_cast<const char*>(u8"머터리얼"));
		CreateFolder(mCurrentLogicalFolder, reinterpret_cast<const char*>(u8"텍스처"));
	//	CreateFolder(mLogicalFolderVector[2]);
	//	CreateFolder(mLogicalFolderVector[1]);
		//CreateFolder(mLogicalFolderVector[3]);
	//	CreateFolder(mLogicalFolderVector[2]);













	}

	void FileUiUiSystem::Update(float deltaTime,bool playMode)
	{
		System::Update(deltaTime,playMode);



	}

	void FileUiUiSystem::OnResize(UINT clientWidth, UINT clientHeight)
	{
		System::OnResize(clientWidth, clientHeight);
		D3D12_VIEWPORT viewport = GetViewPort();
		viewport.TopLeftY = ((DockingWindowController*)GetController())->GetTitleBarHeight();
		//viewport.TopLeftY = 60;
		SetViewPort(viewport.TopLeftX, viewport.TopLeftY, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth);


		Camera* camera = GetMap()->GetMainCamera();
		OrthogoanlCamera* mainCamera = static_cast<OrthogoanlCamera*>(camera);
		mainCamera->SetPosition((float)clientWidth / 2.0f, -1.0f * clientHeight / 2.0f, 0.0f);
		mainCamera->SetViewWidthAndHeight(clientWidth, clientHeight);
		//ReSizeContentItemPanel();

	

		
	}

	void FileUiUiSystem::ReSizeContentItemPanel(bool updateFlag)
	{

		//float clientWidth = GetClientWidth();
		//float clientHeight = GetClientHeight();
		//D3D12_VIEWPORT viewport = GetViewPort();

		//float contentItemUiEntityVisiableWidth = clientWidth - mFolderListRegion.GetWidth();

		//int maxEntityNumInRow = CaculateMaxEntityNumInRow(contentItemUiEntityVisiableWidth);
		//if (maxEntityNumInRow == 0)
		//	return;

		//LogicalContentItemUiEntityFolderNode* currentNode = mCurrentTextPanel->GetFolderNode();




		//if (maxEntityNumInRow != mMaxContentItemUiEntityNumInRow || updateFlag)
		//{
		//	//엔티티들의 위치를 변경한다 .
		//	int currentContentItemUiEntityNum = currentNode->
		//		mFolder->GetContentItemNum();
		//	const std::vector<ContentItemUiEntity*>& contentItemUiEntityVector = currentNode->mFolder->GetContentItemUiEntityVector();

		//	for (int i = 0; i < currentContentItemUiEntityNum; ++i)
		//	{
		//		int rowNum = i / maxEntityNumInRow;
		//		int columnNum = i % maxEntityNumInRow;

		//		float positionX = 0;
		//		float positionY = 0;
		//		CaculateAssetUiEntityPosition(rowNum, columnNum, positionX, positionY);
		//		contentItemUiEntityVector[i]->SetPosition(positionX, positionY, -1.0f);
		//	}

		//	mMaxContentItemUiEntityNumInRow = maxEntityNumInRow;
		//}


		//LogicalContentItemUiEntityFolder* folder = currentNode->
		//	mFolder;
		//folder->GetScrollBarEntity()->SetScrollBarAvaliableMaxShiftY(clientHeight - viewport.TopLeftY);


		////contentItem들이존재하는 영역에대한 높이
		//float panelActiveHeight = GetCotentItemPannelHeight();

		//folder->GetScrollBarEntity()->SetPanelAvailableMaxShiftY(panelActiveHeight);
	}

	







	void FileUiUiSystem::AddContentItemUiEntity(ContentItemUiEntity* contentItemUiEntity)
	{
		//return;

		//각 asset들이 속한 논리적 폴더에 추가하자.

		Asset* asset = static_cast<Asset*>(contentItemUiEntity->GetAsset());

		unsigned long long destLogicalFolderUniqueID = asset->GetDestLogicalFolderUnqiueID();
		if (destLogicalFolderUniqueID == 0)
		{
			asset->SetDestLogicalFolderUnqiueID(mCurrentLogicalFolder->GetUniqueID());
			mCurrentLogicalFolder->AddContentItemUiEntity(contentItemUiEntity);
			return;
		}


		for (auto& logicalFolder : mLogicalFolderVector)
		{

			if (logicalFolder->GetUniqueID() == asset->GetDestLogicalFolderUnqiueID())
			{
				logicalFolder->AddContentItemUiEntity(contentItemUiEntity);
				break;
			}
		}




		//mCurrentLogicalFolder->AddContentItemUiEntity(contentItemUiEntity);

























		//LogicalContentItemUiEntityFolderNode* currentNode = mCurrentTextPanel->GetFolderNode();
		//LogicalContentItemUiEntityFolder* folder = currentNode->
		//	mFolder;
		//contentItemUiEntity->GetTransform().SetIndependentScaleFlag(true);
		//GetMap()->AddObject(contentItemUiEntity);
		//GetMap()->ChangeParentObject(folder->GetPanelUiEntity(), contentItemUiEntity);

		////현재폴더에 넣는다.
		////현재폴더의 엔티티개수를 얻는다 ,각 폴더의 판 크기는 모든 폴더가동일함으로
		////여기서 새로 추가될 엔티티의 위치를 계산하고 지정하고, 그 폴더의 자식으로 맵에 넣어준다.
		//int currentContentItemUiEntityNum = folder->GetContentItemNum();
		//

		//int rowNum = currentContentItemUiEntityNum / mMaxContentItemUiEntityNumInRow;
		//int columnNum = currentContentItemUiEntityNum % mMaxContentItemUiEntityNumInRow;


		////행과 열번호를 넘겨서 위치 계산먼저수행한다.
		//float positionX = 0;
		//float positionY = 0;
		//CaculateAssetUiEntityPosition(rowNum, columnNum, positionX, positionY);



		//contentItemUiEntity->SetPosition(positionX, positionY, -1.0f);
		////contentItemUiEntity->SetPosition()
		//
		//currentNode->mFolder->AddContentItemUiEntity(contentItemUiEntity);




		////contentItem들이존재하는 영역에대한 높이
		//float panelActiveHeight = GetCotentItemPannelHeight();

		//folder->GetScrollBarEntity()->SetPanelAvailableMaxShiftY(panelActiveHeight);



	}

	void FileUiUiSystem::AddAsset(const std::vector<Asset*> & assetVector)
	{

		Map* map = GetMap();

		for (auto& asset : assetVector)
		{
			ContentItemUiEntity* assetUiEntity = ContentItemUiEntity::Create(map, 0, asset, ContentItemUiEntity::EType::eFileUiVersion);
			//assetUiEntity->Initialize(assetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
			AddContentItemUiEntity(assetUiEntity);
		}
	}

	//void FileUiUiSystem::GetEntity(std::vector<Object*>& oEntityVector)
	//{
	//	//GetMap()->GetObjectVectorInViewFrustum(oEntityVector);
	//}

	void FileUiUiSystem::HandleEvent(Event* event)
	{
		//추가하라는 요청이들어온다.
		System::HandleEvent(event);
		const std::string& eventName = event->GetName();





	}

	void FileUiUiSystem::AddEntity(Object* entity, int mapLayerID, bool engineEditObject)
	{
	//	GetMap()->AddObject(entity,mapLayerID);


	}

	//const std::vector<Quad::MapLayer>& FileUiUiSystem::GetEntity()
	//{
	//	// TODO: 여기에 return 문을 삽입합니다.
	//}

	float FileUiUiSystem::GetCotentItemPannelHeight() const
	{
		//int currentContentItemUiEntityNum =  mCurrentLogicalContentItemUiEntityFolderNode->mFolder->GetContentItemNum();
		//int rowNum = currentContentItemUiEntityNum / mMaxContentItemUiEntityNumInRow;
		////int columnNum = currentContentItemUiEntityNum % mMaxContentItemUiEntityNumInRow;

		//return rowNum * (mDistanceBetweenContentItemUiEntity + mContentItemUiEntityHeightSize) + mStartPositionY;

		return 1;
	}

	

	void FileUiUiSystem::CreateFolder(LogicalContentItemUiEntityFolder* logicalContentItemUiEntityParentFolder,const std::string & newFolderName)
	{

		Map* map = GetMap();
		int mapLayerIndex = 0;
		static int i = 0;
		
	//	const std::string newFolderName = "newFolder(" + std::to_string(i)+")";
		//const std::wstring newFolderName = L"newFolder(" + std::to_wstring(i)+L")";
		i++;

		//부모의 폴더항목판넬을 가져온다.
		FolderPanelEntity* parentFolderPanelEntity = logicalContentItemUiEntityParentFolder->GetLinkedFolderPanelEntity();

		//새로생성하는 폴더항목판넬을 생성한다
		FolderPanelEntity * folderPanelEntity =	mFolderListPanelEntity->CreateFolderPanelEntity(newFolderName, parentFolderPanelEntity);


		//연결시킨다 새로운폴더생성과동시에
		LogicalContentItemUiEntityFolder* newFolder = new LogicalContentItemUiEntityFolder(newFolderName, this, folderPanelEntity);

		folderPanelEntity->SetLogicalFolderEntity(newFolder);


		mLogicalFolderVector.push_back(newFolder);

		
		
		//ContentItemUiEntity* folderContentItemUiEntity = ContentItemUiEntity::Create(this, newFolder, ContentItemUiEntity::EType::eFileUiVersion);
		//ContentItemUiEntity* folderContentItemUiEntity = static_cast<ContentItemUiEntity*>(map->CreateObject("ContentItemUiEntity"));
		ContentItemUiEntity* folderContentItemUiEntity = ContentItemUiEntity::Create(map, mapLayerIndex, newFolder, ContentItemUiEntity::EType::eFileUiVersion);
		//folderContentItemUiEntity->Initialize(newFolder, ContentItemUiEntity::EType::eFileUiVersion);
		SetCurrentFolder(logicalContentItemUiEntityParentFolder);

		Asset * folderAsset =static_cast<Asset*>(folderContentItemUiEntity->GetAsset());
		folderAsset->SetDestLogicalFolderUnqiueID(logicalContentItemUiEntityParentFolder->GetUniqueID());

		AddContentItemUiEntity(folderContentItemUiEntity);


	}

	void FileUiUiSystem::SetCurrentFolder(LogicalContentItemUiEntityFolder* folder, bool navigationPanelFlag)
	{

		if (mCurrentLogicalFolder == folder)
			return;



		LogicalContentItemUiEntityFolder* preFolder = nullptr;
		if (mCurrentLogicalFolder != nullptr)
		{


			mCurrentLogicalFolder->ActivateContentItemUiPanelEntity(false);
			mCurrentLogicalFolder->GetLinkedFolderPanelEntity()->SetColorItensity({ 1.0F,1.0F,1.0F });
			mCurrentLogicalFolder->GetLinkedFolderPanelEntity()->GetStateComponent()->SetSelectState(false);

			preFolder = mCurrentLogicalFolder;
		}
		mCurrentLogicalFolder = folder;
		mCurrentLogicalFolder->ActivateContentItemUiPanelEntity(true);
		mCurrentLogicalFolder->GetLinkedFolderPanelEntity()->SetColorItensity({ 0.5F,0.5F,0.5F });



		if (navigationPanelFlag)
		{
			//아무것도 하지않는다.


		}
		else
		{
			//기존의 폴더를 navigation panel의 backFolderDeque에 넣어주고
			//foward folderDeque은 리셋한다.
			if (preFolder != nullptr)
				mNavigationFolderPanelEntity->SetNewFolder(preFolder);

		}









	}

	LogicalContentItemUiEntityFolder* FileUiUiSystem::GetCurrentFolder() const
	{
		return mCurrentLogicalFolder;
	}

	const std::vector<LogicalContentItemUiEntityFolder*>& FileUiUiSystem::GetLogicalContentItemUiEntityFolderVector()
	{
		auto instance = GetInstance();
		return instance->mLogicalFolderVector;
		// TODO: 여기에 return 문을 삽입합니다.
	}

	LogicalContentItemUiEntityFolder* FileUiUiSystem::GetLogicalContentItemUiEntityFolder(const std::string& name)
	{

		const auto& logicalFolderVector = GetLogicalContentItemUiEntityFolderVector();
		std::vector< LogicalContentItemUiEntityFolder*>::const_iterator citr = std::find_if(logicalFolderVector.cbegin(), logicalFolderVector.cend(), [&name](LogicalContentItemUiEntityFolder* folder)
			{
				if (folder->GetName() == name)
					return true;
				return false;
			});

		return citr != logicalFolderVector.cend() ? *citr : nullptr;
	}

	const std::string FileUiUiSystem::GetLogicalFolderPath(LogicalContentItemUiEntityFolder* folder)
	{
		std::string path;
		LogicalContentItemUiEntityFolder* currLogicalFolder = folder;


		LogicalContentItemUiEntityFolder* rootAssetLogicalFolder = mLogicalFolderVector[0];


		while (1)
		{
			path = currLogicalFolder->GetName()+path;

			if (currLogicalFolder == rootAssetLogicalFolder)
				break;

			FolderPanelEntity* folderPanelEntity = currLogicalFolder->GetLinkedFolderPanelEntity();
			FolderPanelEntity* folderPanelEntityParent = static_cast<FolderPanelEntity*>(folderPanelEntity->GetParentObject());

			currLogicalFolder = folderPanelEntityParent->GetLinkedLogicalFolder();
			path = "/" + path;
		}
		
		return path;
	
	}



	void FileUiUiSystem::CreateRootFolder()
	{
		FolderPanelEntity* folderPanelEntity = mFolderListPanelEntity->CreateRootFolderPanelEntity("Asset");

		LogicalContentItemUiEntityFolder* newFolder = new LogicalContentItemUiEntityFolder("Asset", this, folderPanelEntity);

		newFolder->ActivateContentItemUiPanelEntity(true);

		mLogicalFolderVector.push_back(newFolder);

		folderPanelEntity->SetLogicalFolderEntity(newFolder);

		mCurrentLogicalFolder = newFolder;


		folderPanelEntity->SetSelectState();

	}



	int FileUiUiSystem::CaculateMaxEntityNumInRow(int clientWidth)
	{
		return (clientWidth - mStartPositionX ) / (mContentItemUiEntityWidthSize + mDistanceBetweenContentItemUiEntity);
	}

	void FileUiUiSystem::CaculateAssetUiEntityPosition(int rowNum, int columnNum, 
		float & oPositionX, float & oPositionY)
	{

		oPositionX = (columnNum) * (mDistanceBetweenContentItemUiEntity + mContentItemUiEntityWidthSize);
		oPositionY = -1.0f * rowNum * (mDistanceBetweenContentItemUiEntity + mContentItemUiEntityHeightSize);

		
		oPositionX += -mContentItemUiEntityPanelWidth/2.0f + mStartPositionX + mContentItemUiEntityWidthSize / 2.0f;;
		oPositionY += mContentItemUiEntityPanelHeight/2.0f - mStartPositionY + -mContentItemUiEntityHeightSize / 2.0f;;
	//	oPositionY *= -1.0f;

	}





}