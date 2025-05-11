#include "WindowActiveEvent.h"

Quad::WindowActiveEvent::WindowActiveEvent()
	:Event("WindowActive")
{
}

void Quad::WindowActiveEvent::SetWindowActiveState(int state)
{

	mActiveState = state;
}

int Quad::WindowActiveEvent::GetWindowActiveState() const
{
	return mActiveState;
}
