#pragma once

#include"Predeclare.h"
#include"PanelUiEntity.h"


#include"EditorMacro.h"


namespace Quad
{

	//file uiui에서사용하는
	class FolderListPanelEntity:public PanelUiEntity
	{
	public:
		FolderListPanelEntity(const std::string& name="");
		virtual ~FolderListPanelEntity();

		virtual void Initialize() override;
		
		static FolderListPanelEntity* Create(Map* map, int mapLayerIndex = 0);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		CLASSNAME(FolderListPanelEntity)
		CLASSSIZE(FolderListPanelEntity)
		FolderPanelEntity* CreateRootFolderPanelEntity(const std::string& initName);

		FolderPanelEntity* CreateFolderPanelEntity(const std::string& initName , FolderPanelEntity * parentFolderPanelEntity );//부모가 nullptr이면 이 폴더리스트panel이 부모가된다 .하지만이경우는 맨초기 asset root folder만 해당될것이다.
		FolderPanelEntity* CreateFolderPanelEntity(const std::string& initName, const std::wstring  & parentFolderPanelEntityName);


		const FolderPanelEntity* GetFolderPanelEntity(const std::string& folderName) const;
		FolderPanelEntity* GetFolderPanelEntity(const std::string& folderName);

		void ReBuildFolderPanelEntities(FolderPanelEntity* destFolderPanelEntity, bool folderAddFlag);//folderAddFlag :True == 폴더추가, 
		//false == 폴더펼치기
	protected:
		void InitCreating();


	private:
		std::vector<FolderPanelEntity*> mFolderPanelEntityVector;
		FolderPanelEntity* mRootFolderPanelEntity;
		float mFolderPanelHeight = 40.0f;
		float mChildPanelShiftX = 20.0f;


	};
	REGISTEREDITORCLASS(FolderListPanelEntity)
}