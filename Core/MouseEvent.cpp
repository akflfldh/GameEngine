#include "Core/MouseEvent.h"
#include<Windows.h>
#include"Mouse.h"
#include"Object/Object.h"
#include"System.h"
namespace Quad
{
	MouseEvent::MouseEvent(const std::string& name)
		:Event(name), mGlobalEventFlag(true)
	{

		Object* capturedObject = Mouse::GetCapturedObject();
		if (capturedObject == nullptr)
			return;

		SetSystemID(capturedObject->GetSystem()->GetSystemID());


	}

	void MouseEvent::SetClientPosition(int positionX, int positionY)
	{
		mClientPositionX = positionX;
		mClientPositionY = positionY;
	}

	int MouseEvent::GetClientPositionX() const
	{
		return mClientPositionX;
	}

	int MouseEvent::GetClientPositionY() const
	{
		return mClientPositionY;
	}
	void MouseEvent::SetPreClientPosition(int positionX, int positionY)
	{
		mPreClientPositionX = positionX;
		mPreClientPositionY = positionY;
	}
	int MouseEvent::GetPreClientPositionX() const
	{
		return mPreClientPositionX;
	}
	int MouseEvent::GetPreClientPositionY() const
	{
		return mPreClientPositionY;
	}
	void MouseEvent::SetScreenPosition(int positionX, int positionY)
	{
		mScreenPositionX = positionX;
		mScreenPositionY = positionY;
	}
	void MouseEvent::SetPreScreenPosition(int positionX, int positionY)
	{
		mPreScreenPositionX = positionX;
		mPreScreenPositionY = positionY;
	}
	int MouseEvent::GetScreenPositionX() const
	{
		return mScreenPositionX;
	}
	int MouseEvent::GetScreenPositionY() const
	{
		return mScreenPositionY;
	}
	int MouseEvent::GetPreScreenPositionX() const
	{
		
		return mPreScreenPositionX;
	}
	int MouseEvent::GetPreScreenPositionY() const
	{
		return mPreScreenPositionY;
	}
	void MouseEvent::SetRay(Ray ray)
	{
		mRay = ray;
	}
	Ray MouseEvent::GetRay() const
	{
		return mRay;
	}
	void MouseEvent::SetHoverState(bool state)
	{
		mHoverState = state;
	}
	bool MouseEvent::GetHoverState() const
	{
		return mHoverState;
	}
	void MouseEvent::SetGlobalFlag(bool flag)
	{
		mGlobalEventFlag = flag;

	}
	bool MouseEvent::GetGlobalFlag() const
	{
		return mGlobalEventFlag;
	}
	void MouseEvent::SetWheelShift(int shift)
	{
		mWheelShift = shift;
	}
	int MouseEvent::GetWheelShift() const
	{
		return mWheelShift;
	}
}