#include "Core/SelectEvent.h"

Quad::SelectEvent::SelectEvent()
	:Event(""), mSelectedObject(nullptr)
{
	SetEventID(EEventID::eSelectEvent);



}

Quad::SelectEvent::~SelectEvent()
{
}

void Quad::SelectEvent::SetSelectedObject(Object* object)
{
	mSelectedObject = object;
}

Quad::Object* Quad::SelectEvent::GetSelectedObject() const
{
	return mSelectedObject;
}

void Quad::SelectEvent::SetExclusiveEventHandleFlag(bool flag)
{
	mExclusiveEventHandleFlag = flag;
}

bool Quad::SelectEvent::GetExclusiveEventHandleFlag() const
{
	return mExclusiveEventHandleFlag;
}

void Quad::SelectEvent::SetSelectState(bool state)
{
	mSelectState = state;
}

bool Quad::SelectEvent::GetSelectState() const
{
	return mSelectState;
}
