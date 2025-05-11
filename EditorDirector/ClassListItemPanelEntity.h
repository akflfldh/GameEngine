#pragma once

#include"Predeclare.h"
#include"PanelUiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class ClassListItemPanelEntity:public PanelUiEntity
	{
	public:
		ClassListItemPanelEntity();
		virtual ~ClassListItemPanelEntity();

		virtual void Initialize()override;

		static ClassListItemPanelEntity* Create(Map* map, int mapLayerIndex , const std::string& className, float width, float height);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		virtual void SetStencilRefValue(unsigned char value) override;

		std::string GetEntityClassName() const;



		CLASSNAME(ClassListItemPanelEntity)
		CLASSSIZE(ClassListItemPanelEntity)

	

	protected:
		void InitCreating(Map* map, int mapLayerIndex,const std::string& className, float width, float height);

	private:
		virtual void HandleHover(Event* event) override;
		virtual void HandleLButtonDown(Event* event)override;


		TextBoxPanel* mTextBoxPanel;


	};
	REGISTEREDITORCLASS(ClassListItemPanelEntity)
}
