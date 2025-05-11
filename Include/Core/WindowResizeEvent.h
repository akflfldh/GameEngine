#pragma once

#include"Predeclare.h"
#include"Core/Event.h"

#include"Core/CoreDllExport.h"

namespace Quad
{

	class CORE_API_LIB WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent();
		virtual ~WindowResizeEvent()=default;


		void SetPreWindowSize(UINT width, UINT height);
		void SetWindowSize(UINT width, UINT height);

		UINT GetPreWindowWidth()const;
		UINT GetPreWindowHeight()const;

		UINT GetWindowWidth()const;
		UINT GetWindowHeight()const;


	private:
		UINT mPreWindowWidth;
		UINT mPreWindowHeight;

		UINT mWindowWidth;
		UINT mWindowHeight;


	};

}

