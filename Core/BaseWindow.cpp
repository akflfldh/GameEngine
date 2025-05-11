#include "Core/BaseWindow.h"

#include"Core/MouseEvent.h"
#include"Core/BaseWindowController.h"
#include"Core/Mouse.h"

Quad::BaseWindow::BaseWindow(HINSTANCE hInstance, EWindowType windowID)
	:mHInstance(hInstance),mWindowType(windowID),mClientWidth(600),mClientHeight(800),
	mMaxClientWidth(1200),mMaxClientHeight(1200),mMinClientWidth(200),mMinClientHeight(200), mWindowController(nullptr)
{
	
}


Quad::BaseWindow::~BaseWindow()
{

}

void Quad::BaseWindow::Initialize(BaseWindowController* windowController)
{
	mWindowController = windowController;
}

HINSTANCE Quad::BaseWindow::GetHInstance() const
{
	return mHInstance;
}

HWND Quad::BaseWindow::GetWindowHandle() const
{
	return mWindowHandle;
}

Quad::EWindowType Quad::BaseWindow::GetWndID() const
{
	return mWindowType;
}

void Quad::BaseWindow::SetWindowHandle(HWND handle)
{
	mWindowHandle = handle;
}

void Quad::BaseWindow::SetClientWidth(unsigned short width)
{
	unsigned short maxClientWidth = GetMaxClientWidth();
	unsigned short minClientWidth = GetMinClientWidth();

	if (width > maxClientWidth)
		width = maxClientWidth;
	else if (width < minClientWidth)
		width = minClientWidth;
	

	mClientWidth = width;


}

void Quad::BaseWindow::SetClientHeight(unsigned short height)
{
	unsigned short maxClientHeight= GetMaxClientHeight();
	unsigned short minClientHeight = GetMinClientHeight();

	if (height > maxClientHeight)
		height = maxClientHeight;
	else if (height < minClientHeight)
		height = minClientHeight;

	mClientHeight = height;
}

unsigned short Quad::BaseWindow::GetClientWidth() const
{
	return mClientWidth;
}

unsigned short Quad::BaseWindow::GetClientHeight() const
{
	return mClientHeight;
}

void Quad::BaseWindow::SetMaxClientWidth(unsigned short width)
{
	mMaxClientWidth = width;
}

void Quad::BaseWindow::SetMaxClientHeight(unsigned short height)
{
	mMaxClientHeight = height;
}

unsigned short Quad::BaseWindow::GetMaxClientWidth() const
{
	return mMaxClientWidth;
}

unsigned short Quad::BaseWindow::GetMaxClientHeight() const
{
	return mMaxClientHeight;
}

void Quad::BaseWindow::SetMinClientWidth(unsigned short width)
{
	mMinClientWidth = width;
}

void Quad::BaseWindow::SetMinClientHeight(unsigned short height)
{
	mMinClientHeight = height;
}

unsigned short Quad::BaseWindow::GetMinClientWidth() const
{
	return mMinClientWidth;
}

unsigned short Quad::BaseWindow::GetMinClientHeight() const
{
	return mMinClientHeight;
}

void Quad::BaseWindow::HandleDefaultLButtonDown(WPARAM wParam, LPARAM lParam)
{

	MouseEvent* event = new MouseEvent("LButtonDown");

	POINT preClientPosition = { mClientMousePosX,mClientMousePosY };
	event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
	ClientToScreen(GetWindowHandle(), &preClientPosition);
	event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

	POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	event->SetClientPosition(currClientPosition.x, currClientPosition.y);
	mClientMousePosX = currClientPosition.x;
	mClientMousePosY = currClientPosition.y;

	ClientToScreen(GetWindowHandle(), &currClientPosition);
	event->SetScreenPosition(currClientPosition.x, currClientPosition.y);


	mWindowController->AddEvent(event);

	Mouse::SetLButtonState(true);

}

void Quad::BaseWindow::HandleDefaultRButtonDown(WPARAM wParam, LPARAM lParam)
{
	MouseEvent* event = new MouseEvent("RButtonDown");
	/*      event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
		  event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
		  mClientMousePosX = LOWORD(lParam);
		  mClientMousePosY = HIWORD(lParam);*/

	POINT preClientPosition = { mClientMousePosX,mClientMousePosY };
	event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
	ClientToScreen(GetWindowHandle(), &preClientPosition);
	event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

	POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	event->SetClientPosition(currClientPosition.x, currClientPosition.y);
	mClientMousePosX = currClientPosition.x;
	mClientMousePosY = currClientPosition.y;

	ClientToScreen(GetWindowHandle(), &currClientPosition);
	event->SetScreenPosition(currClientPosition.x, currClientPosition.y);
	mWindowController->AddEvent(event);

	Mouse::SetRButtonState(true);
	OutputDebugString(L"RButtonDownOtherWindow\n");


}

void Quad::BaseWindow::HandleDefaultLButtonUp(WPARAM wParam, LPARAM lParam)
{
	MouseEvent* event = new MouseEvent("LButtonUp");
	/*  event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
	  event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
	  mClientMousePosX = LOWORD(lParam);
	  mClientMousePosY = HIWORD(lParam);*/
	POINT preClientPosition = { mClientMousePosX,mClientMousePosY };
	event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
	ClientToScreen(GetWindowHandle(), &preClientPosition);
	event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

	POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	event->SetClientPosition(currClientPosition.x, currClientPosition.y);
	mClientMousePosX = currClientPosition.x;
	mClientMousePosY = currClientPosition.y;

	ClientToScreen(GetWindowHandle(), &currClientPosition);
	event->SetScreenPosition(currClientPosition.x, currClientPosition.y);

	mWindowController->AddEvent(event);

	Mouse::SetLButtonState(false);

}

void Quad::BaseWindow::HandleDefaultRButtonUp(WPARAM wParam, LPARAM lParam)
{
	Mouse::SetRButtonState(false);

	MouseEvent* event = new MouseEvent("RButtonUp");
	/*  event->SetPreClientPosition(mClientMousePosX, mClientMousePosY);
	  event->SetClientPosition(LOWORD(lParam), HIWORD(lParam));
	  mClientMousePosX = LOWORD(lParam);
	  mClientMousePosY = HIWORD(lParam);*/
	POINT preClientPosition = { mClientMousePosX,mClientMousePosY };
	event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
	ClientToScreen(GetWindowHandle(), &preClientPosition);
	event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

	POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	event->SetClientPosition(currClientPosition.x, currClientPosition.y);
	mClientMousePosX = currClientPosition.x;
	mClientMousePosY = currClientPosition.y;

	ClientToScreen(GetWindowHandle(), &currClientPosition);
	event->SetScreenPosition(currClientPosition.x, currClientPosition.y);

	mWindowController->AddEvent(event);


}

void Quad::BaseWindow::HandleDefaultMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (mWindowController != nullptr)
	{


		MouseEvent* event = new MouseEvent("MouseMove");

		POINT preClientPosition = { mClientMousePosX,mClientMousePosY };
		event->SetPreClientPosition(preClientPosition.x, preClientPosition.y);
		ClientToScreen(GetWindowHandle(), &preClientPosition);
		event->SetPreScreenPosition(preClientPosition.x, preClientPosition.y);

		POINT currClientPosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		event->SetClientPosition(currClientPosition.x, currClientPosition.y);
		mClientMousePosX = currClientPosition.x;
		mClientMousePosY = currClientPosition.y;

		ClientToScreen(GetWindowHandle(), &currClientPosition);
		event->SetScreenPosition(currClientPosition.x, currClientPosition.y);


		mWindowController->AddEvent(event);
	}
}


Quad::BaseWindowController* Quad::BaseWindow::GetWindowController()
{
	return mWindowController;
}

