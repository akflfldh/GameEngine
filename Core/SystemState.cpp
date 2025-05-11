#include "Core/SystemState.h"



namespace Quad
{
	SystemState::SystemState(const std::string& name)
		:mName(name), mEventFocusFlag(false)
	{

	}

	const std::string& SystemState::GetName() const
	{
		return mName;
		// TODO: 여기에 return 문을 삽입합니다.
	}

	bool SystemState::GetEventFocusFlag() const
	{
		return mEventFocusFlag;
	}

	void SystemState::SetEventFocusFlag(bool flag)
	{
		mEventFocusFlag = flag;
	}



}