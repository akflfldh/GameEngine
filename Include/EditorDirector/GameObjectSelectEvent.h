#pragma once

#include"Predeclare.h"

#include"Event.h"


namespace Quad
{
	class GameObjectSelectEvent:public Event 
	{
	public:

		GameObjectSelectEvent();
		virtual ~GameObjectSelectEvent()=default;
	

		void SetSelectedObject(Object* object);
		Object* GetSelectObject() const;
		
		void SetCurrentSubMeshIndex(int index);
		int GetCurrentSubMeshIndex() const;
	private:
		Object* mObject;
		int mCurrentSubMeshIndex = 0;

	};


}
