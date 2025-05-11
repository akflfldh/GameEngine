#pragma once




#include"Predeclare.h"
#include"PanelUiEntity.h"
#include<deque>
#include"ButtonUiEntitiy.h"
#include<vector>
#include"EditorMacro.h"
namespace Quad
{

	class NavigationFolderPanelEntity:public PanelUiEntity
	{
	public:
		NavigationFolderPanelEntity(const std::string& name="");
		virtual ~NavigationFolderPanelEntity();

		virtual void Initialize() override;

		static NavigationFolderPanelEntity* Create(Map* map, int mapLayerIndex, float width, float height);

		virtual void Start() override;
		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		CLASSNAME(NavigationFolderPanelEntity)
		CLASSSIZE(NavigationFolderPanelEntity)
		void BackFolder();
		void FowardFolder();

		//navigationFolderPanel의 버튼이 아닌 외부조작으로 새로운 폴더가 설정됫을때 호출
		//foward deque리셋,기존현재폴더를 backDeque으로
		void SetNewFolder(LogicalContentItemUiEntityFolder * preFolder);
	protected:
		void InitCreating(float width, float height);

	private:

		void SetButtonCallback();



		std::deque<LogicalContentItemUiEntityFolder*> mBackFolderEntityDeque;
		std::deque<LogicalContentItemUiEntityFolder*> mFowardFolderEntityDeque;

		ButtonUiEntity* mBackButton;
		ButtonUiEntity* mFowardButton;

		std::vector< LogicalContentItemUiEntityFolder*>mBackFolderEntityVector;
		std::vector< LogicalContentItemUiEntityFolder*>mFowardFolderEntityVector;

		int mCurrentBackVectorIndex;
		int mCurrentFowardVectorIndex;

		int mBackFolderCount;
		int mFowardFolderCount;

		int mVectorMaxCount;
	};
	REGISTEREDITORCLASS(NavigationFolderPanelEntity)
}