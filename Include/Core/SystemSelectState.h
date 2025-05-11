#pragma once

#include"Predeclare.h"

#include"SystemState.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	//특정 엔티티를 마우스로 누르고 띄어서 선택한상태 
	//꾹 누르고있는상태는아니기에 마우스로 그 엔티티를 움직일수는없겠다.
	class CORE_API_LIB SystemSelectState:public SystemState
	{
	public:
		SystemSelectState(const std::string& name);
		~SystemSelectState() = default;


		virtual void EnterState(System& system);
		virtual void RunState(System& system);
		virtual void ExitState(System& syetem);

		void SetSelectedEntity(UiEntity* entity);
		UiEntity* GetSelectedEntity() const;

		virtual SystemState* HandleEvent(System& syetem, Event * event);


	private:
		SystemState* HandleKeyDown(System& system, Event* event);
		SystemState* HandleKeyUp(System& system, Event* event);

		SystemState* HandleLButtonDown(System& system, Event* event);
		SystemState* HandleMouseMove(System& system, Event* event);
		SystemState* HandleMouseInOut(System& system, Event* event);


	private:
		UiEntity* mSelectedEntity;
		Object* mHoverObject;
	};


}

