#include "Event/EventTable.h"

namespace Quad
{
	

	EventTable::EventTable()
	{

		RegisterEvent("MouseMove");
		RegisterEvent("LButtonDown");
		RegisterEvent("LButtonUp");
		RegisterEvent("Select");
		RegisterEvent("ReleaseSelect");
		RegisterEvent("WindowResizeEvent");
		RegisterEvent("KeyDown");

	}
	bool EventTable::RegisterEvent(const std::string& eventName)
	{
		//auto instance = GetInstance();

		//std::pair<std::unordered_map<std::string,std::list<Object*>>::iterator,bool> ret = 
		//	mEventTable.insert({eventName, std::list<Object*>{}});

		std::pair<std::unordered_map<std::string, std::vector<Object*>>::iterator, bool> ret =
			mEventTable.insert({ eventName, std::vector<Object*>{} });

		return ret.second;
	}

	void EventTable::AddObject(const std::string& eventName, Object* object) 
	{
		//std::unordered_map<std::string, std::list<Object*>>::iterator it = mEventTable.find(eventName);
		std::unordered_map<std::string, std::vector<Object*>>::iterator it = mEventTable.find(eventName);
		//if (it == mEventTable.end())
		//{
		//	RegisterEvent(eventName);
		//	mEventTable
		//}
		//else
		//{
		//	RegisterEvent(eventName);
		//}

		mEventTable[eventName].push_back(object);


		//디버그에서는 중복해서 object를 넣는경우를검사하는 코드를 넣어보자

		return;
	}

	void EventTable::RemoveObject(const std::string& eventName, Object* object)
	{
		std::vector<Object*>::iterator objectVectorIt = std::find(mEventTable[eventName].begin(), mEventTable[eventName].end(), object);

		if (objectVectorIt == mEventTable[eventName].end())
			return;


		*objectVectorIt = mEventTable[eventName].back();
		mEventTable[eventName].pop_back();

	}

	void EventTable::GetObjectList(const std::string& eventName, std::list<Object*>& oObjectList)  const
	{


	}

	void EventTable::GetObjectVector(const std::string& eventName, std::vector<Object*>& oObjectVector) const
	{

		std::unordered_map<std::string, std::vector<Object*>>::const_iterator it = mEventTable.find(eventName);

		if (it != mEventTable.end())
		{
			oObjectVector = it->second;
		}

		return;
	}

}


