#pragma once


#include"Predeclare.h"
#include"Core/PanelUiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class ExpandablePanel:public PanelUiEntity
	{
	public:
		ExpandablePanel(const std::string& name="");
		virtual ~ExpandablePanel();

		virtual void Initialize() override;
	
		static ExpandablePanel* Create(Map* map, int mapLayerIndex, float width, float height);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		void AddChildPanelUiEntity(UiEntity* childPanel, const std::string& tagName);
		CLASSNAME(ExpandablePanel)
		CLASSSIZE(ExpandablePanel)
		//true : 접기 ,false: 펼치기
		void CollapsePanel(int panelIndex, bool collapseState);

	protected:

		void InitCreating(float width, float height);

	private:
		virtual void AddChildPanelUiEntity(UiEntity* childPanel) override;

		float mNextPanelLocalPosY; 

	};
	REGISTEREDITORCLASS(ExpandablePanel)

}
