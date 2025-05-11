#pragma once


#include"Predeclare.h"
#include"UiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class BoundaryStick:public UiEntity
	{
	public:
		BoundaryStick(const std::string & name);
		virtual ~BoundaryStick();

		virtual void Initialize() override;

		static BoundaryStick* Create(Map* map, int mapLayerIndex, int direction);




		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event* event) override;

		void SetDstEntity(UiEntity* entity);

	private:

		//event

		void HandleSelectEvent(Event * event);
		void HandleReleaseSelectEvent(Event* event);
		void HandleMouseMoveEvent(Event* event);

	protected:
		void InitCreating(int direction);


	private:
		ResizeHandle* mResizeHandle;
		UiEntity* mDstEntity;
		int mDirection;			//0 left, 1 right ,2 top ,3 bottom

	};
	//REGISTEREDITORCLASS(BoundaryStick)

}

