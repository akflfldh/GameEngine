#pragma once


#include"Predeclare.h"


#include"Core/System.h"


namespace Quad
{
	class DragAndDropSystem :public System
	{
	public:
		DragAndDropSystem();
		virtual ~DragAndDropSystem()=default;

		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map) override;
		virtual void Update(float deltaTime, bool playMode=true) override;
		virtual void OnResize(UINT clientWidth, UINT clientHeight) override;

		virtual void GetRenderSettingItem(RenderSettingItem& oRenderSettingItem)  const override;

		virtual void AddEntity(Object* entity, int mapLayer = 0, bool egineEditObject = false) override;
		//virtual void GetEntity(std::vector<Object*>& oEntityVector) override;
		//virtual const std::vector<MapLayer>& GetEntity()override;
		virtual void HandleEvent(Event* event) override;

	private:



	};
}

