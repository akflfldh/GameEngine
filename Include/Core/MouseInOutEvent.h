#pragma once

#include"Predeclare.h"

#include"Core/Event.h"

#include"Core/CoreDllExport.h"

namespace Quad
{
	enum class EMouseInOut
	{
		eIn=0,
		eOut
	};
	//창밖으로 나갔는가 
	//name MouseInOut
	class CORE_API_LIB MouseInOutEvent:public Event
	{
	public:
		MouseInOutEvent(EMouseInOut state);
		virtual ~MouseInOutEvent();

		EMouseInOut GetMouseState()const;


	private:
		EMouseInOut mState;


	};

}
