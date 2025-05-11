#pragma once

#include"Core/System.h"
#include<string>
#include<vector>

#include"Predeclare.h"

#include"Utility/Singleton.h"
namespace Quad
{

	//fileUiUi시스템이 맵에대한 리스트를 가지고있어야할것이다,
	//논리적으로 폴더가 맵이다. 다만 폴더라는것으로 추상화한다
	//폴더의 계층구조를 vector를 이용
	

	class FileUiUiSystem :public System,public Singleton<FileUiUiSystem>
	{

	public:


	
	public:

		FileUiUiSystem();
		virtual ~FileUiUiSystem();

		void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map) override;
		void Update(float deltaTime,bool playMode) override;

		void OnResize(UINT clientWidth, UINT clientHeight) override;
		void ReSizeContentItemPanel(bool updateFlag = false);//강제로업데이트해야하는지여부(현재폴더를바꿀떄 true가될것이다)
		//void GetRenderSettingItem(RenderSettingItem& oRenderSettingItem) const override;

		void AddContentItemUiEntity(ContentItemUiEntity* contentItemUiEntity);

		//자동으로 들어온 asset에 대해서 contentItemUiEntity를 만들어준다.ㄴ
		void AddAsset(const std::vector<Asset*>& assetVector);


		//void GetEntity(std::vector<Object*>& oUiEntityVector) override;
		void HandleEvent(Event* event) ;
		void AddEntity(Object* entity, int mapLayer = 0, bool egineEditObject = false) override;
	//	virtual const std::vector<MapLayer>& GetEntity()override;

		//contentItem이 있는 행까지의 거리(현재폴더의)
		float GetCotentItemPannelHeight() const;


		
		void CreateFolder(LogicalContentItemUiEntityFolder* parentFolderNode, const std::string& newFolderName);
		//bool navigationPanelFlag는 navigationPanel에있는 back,foward 버튼에의해서 현재폴더 바뀌는지의 여부를나타낸다.
		void SetCurrentFolder(LogicalContentItemUiEntityFolder* folder, bool navigationPanelFlag = false);
		LogicalContentItemUiEntityFolder* GetCurrentFolder() const;


		static const std::vector<LogicalContentItemUiEntityFolder*>& GetLogicalContentItemUiEntityFolderVector();
		static LogicalContentItemUiEntityFolder* GetLogicalContentItemUiEntityFolder(const std::string& name);
	

		const std::string GetLogicalFolderPath(LogicalContentItemUiEntityFolder* folder);


	private:
		void CreateRootFolder();



	private:
		int CaculateMaxEntityNumInRow(int clientWidth);
		void CaculateAssetUiEntityPosition(int rowNum, int columnNum,
			float& oPositionX, float& oPositionY);
		

	//	void ChangeCurrentFolder();
	
	private:
		float mContentItemUiEntityWidthSize = 100.0f;
		float mContentItemUiEntityHeightSize = 140.0f;
		float mDistanceBetweenContentItemUiEntity = 50.f;
		float mStartPositionX = 50.0f;
		float mStartPositionY = 50.0f;
		UINT mMaxContentItemUiEntityNumInRow;
		UINT mCurrentContentItemUiEntityNum;
	
		TextPanelEntity* mRootTextPanel;
		TextPanelEntity * mCurrentTextPanel;

		float mContentItemUiEntityPanelWidth=3000;
		float mContentItemUiEntityPanelHeight=2000;

		FolderListPanelEntity* mFolderListPanelEntity;
		//ContentItemUiPanelEntity mContentItemUiPanelEntity;


		std::vector<LogicalContentItemUiEntityFolder*> mLogicalFolderVector;
		LogicalContentItemUiEntityFolder* mCurrentLogicalFolder =nullptr;


		float mFoderFanelStartOffsetY = -40.0f;


		NavigationFolderPanelEntity* mNavigationFolderPanelEntity;

	};


}