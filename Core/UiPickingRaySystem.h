#pragma once

#include"Predeclare.h"
#include"Core/CoreDllExport.h"
#include<vector>

namespace Quad
{
	class Event;
	
	class CORE_API_LIB UiPickingRaySystem
	{
	public:
		UiPickingRaySystem();
		~UiPickingRaySystem();
	
		bool HandleEvent(Event * pEvent, const std::vector<Object*> & objectVector);
			

	private:
		void HandleLButtonDown(MouseEvent* mouseEvent);
		void HandleLButtonUp(MouseEvent* mouseEvent);
		void HandleRButtonDown(MouseEvent* mouseEvent);
		void HandleRButtonUp(MouseEvent* mouseEvent);
		void HandleMouseMove(MouseEvent* mouseEvent);



	};
}
