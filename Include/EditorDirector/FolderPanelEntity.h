#pragma once

#include"Predeclare.h"
#include"PanelUiEntity.h"
#include"ListExpandButton.h"
#include"IconEntity.h"
#include"TextBox.h"
#include"EditorMacro.h"
namespace Quad
{
	class FolderPanelEntity:public PanelUiEntity
	{
	public:
		FolderPanelEntity(const std::string& name="");
		virtual ~FolderPanelEntity();

		virtual void Initialize() override;

		static FolderPanelEntity* Create(Map* map, int mapLayerIndex, float width, float height, const std::string& folderName, FolderListPanelEntity* folderListPanel);


		virtual void Start() override;
		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		CLASSNAME(FolderPanelEntity)
		CLASSSIZE(FolderPanelEntity)
		int GetChildFolderPanelEntityNum() const;
		void IncreaseChildFolderPanelEntityNum();
		void DecreaseChildFolderPanelEntityNum();


		const std::string& GetFolderName() const;


		void ExpandChildFolderPanel();
		void SetFolderPanelValid(bool state);

		int GetChildFolderExpandFlag() const;
		void SetChildFolderExpandFlag(bool flag);
	
		void SetLogicalFolderEntity(LogicalContentItemUiEntityFolder* destLogicalFolder);
		LogicalContentItemUiEntityFolder* GetLinkedLogicalFolder() const;


		//마우스가아닌경우에 선택할수있도록
		void SetSelectState();

	protected:
		void InitCreating(float width, float height, const std::string& folderName, FolderListPanelEntity* folderListPanel);


	private:

		void HandleHover(Event* pEvent) override;
		void HandleLButtonDown(Event* pEvent) override;
		void HandleLButtonUp(Event* pEvent) override;
		void HandleMouseMove(Event* pEvent) override;
		void HandleRButtonUp(Event* pEvent) override;
		void HandleRButtonDown(Event* pEvent) override;
	
		const FolderListPanelEntity* GetFolderListPanelEntity()const;
		FolderListPanelEntity* GetFolderListPanelEntity();
		

	private:
		ListExpandButton* mChildFolderExpandButton;
		IconEntity* mFolderIcon;
		TextBox* mFolderNameText;

		static float mFolderPanelExpandButtonWidth;
		static float mFolderPanelExpandButtonHeight ;
		static float mFolderPanelIconWidth;
		static float mFolderPanelIconHeight;
		static float mTextWidth;
		static float mTextHeight;


		int mChildFolderPanelEntityNum = 0;

		FolderListPanelEntity* mFolderListPanelEntity;

		LogicalContentItemUiEntityFolder* mDestFolderEntity;
		


		
		bool mChangeContentItemPanelFlag;



		//std::wstring mFolderName;
	};
	REGISTEREDITORCLASS(FolderPanelEntity)

}

