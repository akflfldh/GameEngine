#include "Core/Mouse.h"
#include"Object/Object.h"
#include"Core/System.h"
#include"Core/BaseWindow.h"
//#include"Window.h"
#include<windows.h>

#include"Core/InputSystem.h"

bool Quad::Mouse::mLButtonState=false;
bool Quad::Mouse::mRButtonState=false; 

Quad::Mouse::Mouse()
	:mCaptureObject(nullptr), mCurrScreenPosX(0),mCurrScreenPosY(0),mBeforeScreenPosX(0), mBeforeScreenPosY(0)
{


}

void Quad::Mouse::Initlaize(HINSTANCE hInstance)
{

	mHInstance = hInstance;
	mDefaultCursor = LoadCursor(NULL, IDC_ARROW);
	mHorizontalSizeCursor= LoadCursor(NULL, IDC_SIZENS);
	mVerticalSizeCursor= LoadCursor(NULL, IDC_SIZEWE);
	


}

bool Quad::Mouse::GetLButtonState()
{

	return mLButtonState;
}

bool Quad::Mouse::GetRButtonState()
{
	return mRButtonState;
}

void Quad::Mouse::SetLButtonState(bool state)
{
	mLButtonState = state;
	auto inputSystem = InputSystem::GetInstance();
	if (state == true)
	{
		inputSystem->OnEvent(VK_LBUTTON, EInputState::eKeyDown);
	}
	else
	{
		inputSystem->OnEvent(VK_LBUTTON, EInputState::eKeyUp);
	}


}

void Quad::Mouse::SetRButtonState(bool state)
{
	mRButtonState = state;

	auto inputSystem = InputSystem::GetInstance();
	if (state == true)
	{
		inputSystem->OnEvent(VK_LBUTTON, EInputState::eKeyDown);
	}
	else
	{
		inputSystem->OnEvent(VK_RBUTTON, EInputState::eKeyUp);
	}

}

void Quad::Mouse::SetVerticalArrowCursor()
{
	Mouse* instance = GetInstance();
	instance->mArrowCursorState = 2;
	SetCursor(instance->mVerticalSizeCursor);


}

void Quad::Mouse::SetHorizontalArrowCursor()
{
	Mouse* instance = GetInstance();
	instance->mArrowCursorState = 1;
	SetCursor(instance->mHorizontalSizeCursor);

}

void Quad::Mouse::SetDefaultCursor()
{
	Mouse* instance = GetInstance();

	instance->mArrowCursorState = 0;
	SetCursor(instance->mDefaultCursor);

}

void Quad::Mouse::CaptureObject(Object* object)
{
	auto instance = GetInstance();

	System* system = object->GetSystem();
	if (system == nullptr)
		return;


	HWND windowHandle = system->GetWindow()->GetWindowHandle();
	SetCapture(windowHandle);
	
	instance->mCaptureObject= object;

}

void Quad::Mouse::ReleaseCaptureObject(Object* object)
{
	auto instance = GetInstance();

	if (instance->mCaptureObject != object)
		return;


	ReleaseCapture();

	instance->mCaptureObject = nullptr;

}

Quad::Object* Quad::Mouse::GetCapturedObject()
{
	auto instance = GetInstance();

	return instance->mCaptureObject;
}

int Quad::Mouse::GetArrowCursorState()
{
	auto instance = GetInstance();
	return instance->mArrowCursorState;
}

void Quad::Mouse::SetCurrScreenPos(int x, int y)
{
	static bool firstSetting = true;


	auto instance = GetInstance();
	instance->mBeforeScreenPosX = instance->mCurrScreenPosX;
	instance->mBeforeScreenPosY = instance->mCurrScreenPosY;

	if (firstSetting)
	{
		instance->mBeforeScreenPosX = x;
		instance->mBeforeScreenPosY = y;
		firstSetting = false;
	}


	instance->mCurrScreenPosX = x;
	instance->mCurrScreenPosY = y;


}

int Quad::Mouse::GetScreenPosDeltaX()
{
	auto instance = GetInstance();

	int ret = instance->mCurrScreenPosX - instance->mBeforeScreenPosX;
	return ret;
}

int Quad::Mouse::GetScreenPosDeltaY()
{
	auto instance = GetInstance();

	int ret = instance->mCurrScreenPosY - instance->mBeforeScreenPosY;

	return ret;

}

void Quad::Mouse::EndUpdate()
{

	mBeforeScreenPosX = mCurrScreenPosX;
	mBeforeScreenPosY = mCurrScreenPosY;
}



