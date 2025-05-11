#pragma once




#include"Predeclare.h"
#include"System.h"

namespace Quad
{

	class FrameWindowUiSystem:public System
	{
	public:
		FrameWindowUiSystem();
		virtual ~FrameWindowUiSystem() ;

		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map);
		virtual void Update(float deltaTime, bool playMode)override;
		virtual void OnResize(UINT clientWidth, UINT clientHeight);

		virtual void AddEntity(Object* entity, int mapLayer = 0, bool egineEditObject = false);
	//	virtual void GetEntity(std::vector<Object*>& oEntityVector);
	//	virtual const std::vector<MapLayer>& GetEntity()override;
		virtual void HandleEvent(Event* pEvent);
		
	private:



	};


}

