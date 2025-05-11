#pragma once

#include"Predeclare.h"
#include"Core/Event.h"
#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB WindowRequestEvent:public Event
	{
	public:
		WindowRequestEvent(EWindowRequestEventType eventType);
		virtual ~WindowRequestEvent();

		EWindowRequestEventType GetEventType() const;
		
		void SetWindowPositionData(UINT mPositionX, UINT mPositionY);
		UINT GetWindowPositionX()const;
		UINT GetWindowPositionY()const;

		void SetWindowPositionShiftData(int positionXShift, int positionYShift);
		int GetWindowPositionShiftX() const;
		int GetWindowPositionShiftY() const;

	private:
		EWindowRequestEventType mEventType;
		union WindowRequestData
		{
			struct
			{
				float a;
				float b;
			};

			struct 
			{
				UINT mWindowPositionX;
				UINT mWindowPositionY;
			};

			struct
			{
				int mWindowPosXShift;
				int mWindowPosYShift;
			};

			double c;

		};


		WindowRequestData mData;

	};

}
