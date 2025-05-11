#pragma once

#include"Predeclare.h"
#include"Event.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB WindowActiveEvent:public Event
	{
	public:
		WindowActiveEvent();
		virtual ~WindowActiveEvent() =default;

		void SetWindowActiveState(int state);
		int GetWindowActiveState()const;
	
	private:

		int mActiveState;	//0 활성화 1 비활성화

	};

}
