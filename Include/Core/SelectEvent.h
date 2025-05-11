#pragma once

#include"Predeclare.h"
#include"Core/Event.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB SelectEvent: public Event
	{
	public:
		SelectEvent();
		~SelectEvent();

		void SetSelectedObject(Object* object);
		Object* GetSelectedObject() const;

		void SetExclusiveEventHandleFlag(bool flag);
		bool GetExclusiveEventHandleFlag() const;

		void SetSelectState(bool state);
		bool GetSelectState()const;

	private:
		Object* mSelectedObject;
		bool mExclusiveEventHandleFlag = false;
		bool mSelectState=true;

	};

}
