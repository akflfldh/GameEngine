#include "Core/MouseInOutEvent.h"

Quad::MouseInOutEvent::MouseInOutEvent(EMouseInOut state)
	:Event("MouseInOut")
{

	mState = state;

}

Quad::MouseInOutEvent::~MouseInOutEvent()
{
}

Quad::EMouseInOut Quad::MouseInOutEvent::GetMouseState() const
{
	return mState;
}
