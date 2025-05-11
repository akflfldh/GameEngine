#include "Core/Event.h"


namespace Quad
{

	Event::Event(const std::string& name)
		:mName(name)
	{
	}

	Event::~Event()
	{

	}

	const std::string& Event::GetName() const
	{
		return mName;
		// TODO: 여기에 return 문을 삽입합니다.
	}

	void Event::SetSystemID(ESystemID systemID)
	{
		mSystemID = systemID;

		if (mSystemID != ESystemID::eFrameDockingSystem)
		{
			int a = 2;
		}
	}

	ESystemID Event::GetSystemID() const
	{
		return mSystemID;
	}

	bool Event::GetImmediateEventFlag() const
	{
		return mImmediateEventFlag;
	}

	void Event::SetEventID(EEventID id)
	{
		mEventID = id;
	}

	EEventID Event::GetEventID() const
	{
		return mEventID;
	}

	void Event::SetName(const std::string& name)
	{
		mName = name;
	}

	void Event::SetImmediateEventFlag(bool flag)
	{
		mImmediateEventFlag = flag;
	}



	DragAndDropEvent::DragAndDropEvent(const std::string& name)
		:Event(name)
	{
	}

	DragAndDropEvent::~DragAndDropEvent()
	{
	}



	void DragAndDropEvent::SetFilePath(const std::wstring& filePath)
	{
		mFilePath = filePath;
	}

	std::wstring DragAndDropEvent::GetFilePath() const
	{
		return mFilePath;
	}


}