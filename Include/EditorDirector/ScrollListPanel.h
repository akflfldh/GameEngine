#pragma once


#include"Predeclare.h"

#include"PanelUiEntity.h"
#include"EditorMacro.h"
namespace Quad
{

	class ScrollListPanel:public PanelUiEntity
	{
	public:
		ScrollListPanel(const std::string& name="");
		virtual ~ScrollListPanel();

		virtual void Initialize() override;

		static ScrollListPanel* Create(Map* map, int mapLayerIndex, float width, float height, int maxNum);

		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		CLASSNAME(ScrollListPanel)
		CLASSSIZE(ScrollListPanel)
		virtual void AddChildPanelUiEntity(UiEntity* childPanel) override;

		void SetSelectedObject(Object* object);
		Object* GetSelectedObject()const;

		//(1인덱스(0은 스크롤바)부터 num인덱스까지 num개의 자식판넬을 활성화한다.
		void ActivateChildListPanel(int num);

		//특정 자식판넬 비활성화(제거는아니다)
		void InActiveChildPanel(int index);

		void SetDrawFlag(bool flag) override;

		int GetActiveChildPanelNum() const;


		//스크롤바는 맨위로 설정(자동으로 자식판넬들도 첫번째부터 보일것이다)
		void ResetLScrollBar();

		//클릭(LButtonDown)한 자식판넬을 리턴해준다
		UiEntity* GetCurrentSelectedChildPanel()const;
		//0번은없는것
		int GetCurrentSelectedChildPanelIndex()const;

	protected:
		void InitCreating(float width, float height, int maxNum);

	private:
		void HandleMouseMove(Event* pEvent) override;
		void HandleHover(Event* pEvent) override;
		void HandleLButtonDown(Event* pEvent) override;
		void HandleWheel(Event* pEvent);

	//	ScrollBarEntity* mScrollbar;

		PanelUiEntity* mScrollBarPanel;

		static int mStencilValue;
		//스크롤리스트는 생성될때마다 고유의 스텐실값을 가지자 (다른스크롤리스트판넬과 동일하면 리스트들이 다른스크롤리스트판넬에서보이는문제가
		//발생할수있으니
		//(다만 스텐실값을 사용하는 다른 종류의 판넬이있을경우는 어떻할것인가  - 그럼 아예 전역적으로 값을 관리해야할수도있다);



		float mActiveChildPanelHeightSum;
		//스크롤바가움직일때 자식판넬들이 움직이여야한느비율값
		float mChildPanelDeltaShiftY;

		Object* mSelectedObject;
		int mChildPanelMaxNum;

		//현재활성화된 자식판넬의개수
		int mCurrentChildPanelNum;


		//현재선택한 자식판넬인덱스
		int mCurrentSelectChildPanelIndex;
	};
	REGISTEREDITORCLASS(ScrollListPanel)
}
