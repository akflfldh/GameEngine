#pragma once


#include"Predeclare.h"
#include"System/WindowDockingSystem.h"

namespace Quad
{
	class ChildWindowDockingSystem:public WindowDockingSystem
	{
	public:
		ChildWindowDockingSystem(ESystemID systemID);
		virtual ~ChildWindowDockingSystem() = default;

		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map * map) override;
		virtual void Update(float deltaTime, bool playMode) override;
		virtual void OnResize(UINT clientWidth, UINT clientHeight) override;

		virtual void AddEntity(Object* entity, int mapLayer = 0, bool egineEditObject = false) override;
		//virtual void GetEntity(std::vector<Object*>&oEntityVector) override;
		virtual void HandleEvent(Event * event) override;
	//	virtual std::vector<MapLayer>& GetEntity()override;

		void SelectBoxHover(EDockingDirection boxDirection);
		void SelectBoxReleaseHover();


		//박스위에서 LButtonUp
		void SelectBoxLButtonUp(EDockingDirection boxDirection, bool state);

		void SetDockingSystemState(bool state) override;

	private:

		DockingPanel* mDockingPanel;
	};

}

