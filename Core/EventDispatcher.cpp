#include "Core/EventDispatcher.h"
#include"Core/Controller.h"
#include"Core/SelectEvent.h"
void Quad::EventDispatcher::Initialize()
{

}

void Quad::EventDispatcher::RegisterSystem(System* system)
{
	mSystemUnMapTable.insert({ system->GetSystemID(), system });
}

void Quad::EventDispatcher::SendEvent(Event* event, ESystemID systemID)
{
	
	auto instance = GetInstance();
	std::unordered_map<ESystemID, System*>::iterator it = instance->mSystemUnMapTable.find(systemID);
	event->SetSystemID(systemID);


	if (it != instance->mSystemUnMapTable.end())
	{
		System* system = it->second;

		if (!event->GetImmediateEventFlag())
		{
			Controller* controller = system->GetController();
			controller->AddEvent(event);
			return;
		}

		const std::string & eventName  = event->GetName();
		if (eventName == "ReleaseFocusEvent")
		{
			system->SetEventFocusFlag(false);

		}


	}

	delete event;

}

void Quad::EventDispatcher::SendSelectEvent(Object* destObject, bool state, bool exclusiveEventHandleFlag)
{

	
	SelectEvent* selectEvent = new SelectEvent;
	selectEvent->SetSelectState(state);
	selectEvent->SetSelectedObject(destObject);
	selectEvent->SetExclusiveEventHandleFlag(exclusiveEventHandleFlag);

	EventDispatcher::SendEvent(selectEvent, destObject->GetSystem()->GetSystemID());



}
