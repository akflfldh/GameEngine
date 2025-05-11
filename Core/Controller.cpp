#include "Core/Controller.h"


namespace Quad
{
	std::unordered_map<std::string, Controller*> Controller::mControllerUnMap;

	void Controller::Initialize()
	{
		mEventQueue.Initialize(100);
		mEventQueueSynObject.Initialize();
	}

	void Controller::AddEvent(Event* event)
	{
		
		mEventQueueSynObject.WriterStart();

		if (!mEventQueue.isFull())
			mEventQueue.PushBack(event);

		mEventQueueSynObject.WriterEnd();
	}

	bool Controller::AddController(const std::string& name, Controller* controller)
	{
		std::pair<std::unordered_map<std::string,Controller*>::iterator,bool> ret= mControllerUnMap.insert({ name,controller });
		return ret.second;
	}

	void Controller::AddEvent(const std::string& controllerName, Event* event)
	{
		std::unordered_map<std::string, Controller*>::iterator it = mControllerUnMap.find(controllerName);
		if(it!=mControllerUnMap.end())
			it->second->AddEvent(event);
			
		return;
	}

	const std::string& Controller::GetName() const
	{
		return mName;
		// TODO: 여기에 return 문을 삽입합니다.
	}

	void Controller::SetName(const std::string& name)
	{
		mName = name;
	}

	Event* Controller::GetEvent()
	{
		mEventQueueSynObject.WriterStart();
		Event* pEvent = nullptr;
		if (!mEventQueue.isEmpty())
		{
			pEvent = mEventQueue.Front();
			mEventQueue.Pop();

			mEventQueueSynObject.WriterEnd();
			return pEvent;
		}
		mEventQueueSynObject.WriterEnd();
		//WriterEnd();
		return pEvent;
	}

}