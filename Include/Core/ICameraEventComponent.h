#pragma once


#include"Predeclare.h"

namespace Quad
{
	class ICameraEventComponent
	{
	public:
		ICameraEventComponent() = default;
		virtual ~ICameraEventComponent() = 0;

		virtual void OnEvent(Event* pEvent) = 0;
		
	private:
	



	};

}
