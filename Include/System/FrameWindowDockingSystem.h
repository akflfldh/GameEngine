#pragma once

#include"Predeclare.h"
#include"WindowDockingSystem.h"
#include"Allocator/DestructingPoolAllocator.h"
//#include"SizeControllerStickEntity.h"
#include<array>


namespace Quad

{
	class FrameWindowDockingSystem:public WindowDockingSystem
	{
	public:
		FrameWindowDockingSystem();
		virtual ~FrameWindowDockingSystem() = default;

		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map * map) override;
		virtual void Update(float deltaTime, bool playMode) override;
		virtual void OnResize(UINT clientWidth, UINT clientHeight) override;

		virtual void AddEntity(Object* entity, int mapLayer = 0,bool egineEditObject = false) override;
	
		virtual void HandleEvent(Event * event) override;
	//	virtual const std::vector<MapLayer>& GetEntity()override;
		SizeControllerStickEntity* CreateSizeControlStick();
		void ReleaseSizeControlStick(SizeControllerStickEntity * stick);

	private:
	
		static const int mStickMaxNum =50 ;
		std::array<SizeControllerStickEntity*,mStickMaxNum> mStickArray;
		int mStickArrayNextCreateIndex = 0;
		

	};
}

