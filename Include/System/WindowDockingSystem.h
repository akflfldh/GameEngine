#pragma once

#include"Predeclare.h"
#include"Core/System.h"

namespace Quad

{
	class WindowDockingSystem:public System
	{
	public:
		WindowDockingSystem(ESystemID systemID,const std::string & name);
		virtual ~WindowDockingSystem() =default;

		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map);
		virtual void Update(float deltaTime, bool playMode=true)override;
		virtual void OnResize(UINT clientWidth, UINT clientHeight);

		virtual void AddEntity(Object* entity, int mapLayer = 0, bool egineEditObject = false) ;
		//virtual void GetEntity(std::vector<Object*>& oEntityVector) ;
		virtual void HandleEvent(Event* event);
	//	virtual const std::vector<MapLayer>& GetEntity()override;
		virtual void SetDockingSystemState(bool state);
		bool GetDockingSystemState()const;

	
	private:
		bool mDockingSystemState = false;	
	

		float mWidth;
		float mHeight;


	};
}

