#include "UiPickingRaySystem.h"
#include"Map/Map.h"

#include"Event.h"
#include"MouseEvent.h"



Quad::UiPickingRaySystem::UiPickingRaySystem()
{
}

Quad::UiPickingRaySystem::~UiPickingRaySystem()
{
}

bool Quad::UiPickingRaySystem::HandleEvent(Event* pEvent, const std::vector<Object*>& objectVector)
{

	//evnet 구분 

	//objectVector , UiComponent를 가진 object들에대해서 적절한 ray 판정 수행



	int eventID = (int)pEvent->GetEventID();


	if ((eventID < (int)EEventID::eMouseLButtonDownEvent) || (eventID >(int)EEventID::eMouseMoveEvent))
	{
		return false;
	}


	MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);

	switch (pEvent->GetEventID())
	{
	case EEventID::eMouseMoveEvent:

		HandleMouseMove(mouseEvent);
		break;
	case EEventID::eMouseLButtonUpEvent:
		HandleLButtonUp(mouseEvent);
		break;

	case EEventID::eMouseLButtonDownEvent:
		HandleLButtonDown(mouseEvent);
		break;

	case EEventID::eMouseRButtonDownEvent:
		HandleRButtonDown(mouseEvent);

		break;

	case EEventID::eMouseRButtonUpEvent:
		HandleRButtonUp(mouseEvent);

		break;
	}

	return true;









	
}

void Quad::UiPickingRaySystem::HandleLButtonDown(MouseEvent* mouseEvent)
{
}

void Quad::UiPickingRaySystem::HandleLButtonUp(MouseEvent* mouseEvent)
{
}

void Quad::UiPickingRaySystem::HandleRButtonDown(MouseEvent* mouseEvent)
{
}

void Quad::UiPickingRaySystem::HandleRButtonUp(MouseEvent* mouseEvent)
{
}

void Quad::UiPickingRaySystem::HandleMouseMove(MouseEvent* mouseEvent)
{
}
