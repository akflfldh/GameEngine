#pragma once

#include<string>
#include"Core/SystemState.h"


namespace Quad
{
	class  DefaultState :public SystemState
	{
	public:
		DefaultState(const std::string& name);
		~DefaultState() = default;

		virtual void EnterState(System& system) override;
		virtual void RunState(System& system) override;
		virtual void ExitState(System& system) override;

		virtual SystemState* HandleEvent(System& system, Event* event) override;		//이벤트를 처리하고나서
		
		
		virtual void Reset();

	private:
		virtual SystemState* HandleLButtonDownEvent(System& system, Event* event);

		virtual SystemState* HandleKeyDownEvent(System& system, Event* event);
		virtual SystemState* HandleKeyUpEvent(System& system, Event* event);
		virtual SystemState* HandleMouseMoveEvent(System& system, Event* event);
		virtual SystemState* HandleMouseInOutEvent(System& system, Event* event);
		virtual SystemState* HandleLButtonUpEvent(System& system, Event* event);
		virtual void HandleRButtonDownEvent(System& system, Event* event);
		virtual void HandleRButtonUpEvent(System& system, Event* event);
		void HandleLButtonDownDouble(System& system, Event* event);

		void HandleGameObjectSelectEvent(System& system, Event* event);


		void DefaultHandleEvents(System& system, Event* event);


	private:

		Object* mHoverObject=nullptr;

	};

}