#pragma once

#include"Predeclare.h"
#include"Object/UiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class DragAndDropUiEntity:public UiEntity
	{
	public:
		DragAndDropUiEntity();
		virtual ~DragAndDropUiEntity()=default;

		virtual void Initialize();

		DragAndDropUiEntity* Create(Map* map, int mapLayerIndex = 0);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		CLASSNAME(DragAndDropUiEntity)
		CLASSSIZE(DragAndDropUiEntity)
	protected:

		void InitCreating();

	private:
	};
	REGISTEREDITORCLASS(DragAndDropUiEntity)
}

