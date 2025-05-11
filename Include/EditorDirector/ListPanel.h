#pragma once



#include"PanelUiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class ListPanel : public PanelUiEntity
	{
	public:
		ListPanel();
		virtual ~ListPanel();

		virtual void Initialize() override;
		static ListPanel* Create(Map* map, int mapLayerIndex, float width, float height, int childPanelUiEntityHeight);

		virtual void OnEvent(Event* pEvent) override;

		CLASSSIZE(ListPanel)
		CLASSNAME(ListPanel)

		//Object의 AddChildObject와 PanelUiEntity의 AddChildPanelUiEntity메서드는 사용하지말고 이 메서드를 사용해야 올바르게 자식이 추가된다.
		bool AddChildPanelEntity(PanelUiEntity* childPanelUiEntity);

	private:
		void InitCreating(float width, float height, int childPanelUiEntityHeight);




		float CalcChildPanelUiEntityPosLocalY(int childPanelUiEntityIndex);


		virtual void HandleHover(Event* pEvent) override;
		virtual void HandleMouseMove(Event* pEvent) override;

		int mChildListItemMaxNum;
		int mChildListCurrentNum;

		float mChildPanelUiEntityHeight;

	};
	REGISTEREDITORCLASS(ListPanel)
}
