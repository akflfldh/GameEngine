#include"WindowRequestEvent.h"

Quad::WindowRequestEvent::WindowRequestEvent(EWindowRequestEventType eventType)
	:Event("WindowRequestEvent"),mEventType(eventType)
{


}

Quad::WindowRequestEvent::~WindowRequestEvent()
{

}

Quad::EWindowRequestEventType Quad::WindowRequestEvent::GetEventType() const
{
	return mEventType;
}

void Quad::WindowRequestEvent::SetWindowPositionData(UINT mPositionX, UINT mPositionY)
{
	mData.mWindowPositionX = mPositionX;
	mData.mWindowPositionY = mPositionY;

}

UINT Quad::WindowRequestEvent::GetWindowPositionX() const
{
	return mData.mWindowPositionX;
}

UINT Quad::WindowRequestEvent::GetWindowPositionY() const
{
	return mData.mWindowPositionY;
}

void Quad::WindowRequestEvent::SetWindowPositionShiftData(int positionXShift, int positionYShift)
{

	mData.mWindowPosXShift = positionXShift;
	mData.mWindowPosYShift = positionYShift;
}

int Quad::WindowRequestEvent::GetWindowPositionShiftX() const
{
	return mData.mWindowPosXShift;
}

int Quad::WindowRequestEvent::GetWindowPositionShiftY() const
{
	return mData.mWindowPosYShift;
}



