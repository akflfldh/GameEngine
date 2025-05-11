#pragma once



#include"Predeclare.h"
#include"ContentItemBaseUiEntity.h"
#include"EditorMacro.h"
namespace Quad
{

	class ContentIProjecttemUiEntity:public ContentItemBaseUiEntity
	{
	public:
		virtual void Initialize() override;

		static ContentIProjecttemUiEntity* Create(Map* map, int mapLayerIndex, ContentItem* item);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		CLASSNAME(ContentIProjecttemUiEntity)
		CLASSSIZE(ContentIProjecttemUiEntity)




	protected:
		void InitCreating(ContentItem* item);

		virtual void HandleLButtonDown(Event* pEvent) override;
		virtual void HandleLButtonUp(Event* pEvent) override;
		//virtual void HandleRButtonDown(Event* pEvent)  override;
		//virtual void HandleRButtonUp(Event* pEvent) override;
		//virtual void HandleMouseMove(Event* pEvent) override;
		virtual void HandleHover(Event* pEvent) override;
		void HandleLButtonDownDouble(Event* pEvent);

	private:

	};
	REGISTEREDITORCLASS(ContentIProjecttemUiEntity)

}
