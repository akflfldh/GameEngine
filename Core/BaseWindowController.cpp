#include "Core/BaseWindowController.h"
#include"BaseWindowMsgHandlerComponent.h"

Quad::BaseWindowController::BaseWindowController(RenderSystem* renderSystem)
	:Controller(),mMousePositionState(true), mRenderSystem(renderSystem)
{
}

bool Quad::BaseWindowController::GetMousePositionState() const
{
	return mMousePositionState;
}

void Quad::BaseWindowController::SetMousePositionState(bool state)
{
	mMousePositionState = state;
}


void Quad::BaseWindowController::AddMsgHanlderComponent(BaseWindowMsgHandlerComponent* component)
{
	mWindowMsgHandlerComponentVector.push_back(component);
}

LRESULT Quad::BaseWindowController::HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT  ret = 0;
	bool handled = false;
	for (auto& handler : mWindowMsgHandlerComponentVector)
	{
		bool handledTemp = false;
		LRESULT retTemp =	handler->HandleMessage(hwnd, msg, wParam, lParam, handledTemp);

		if (handledTemp)
		{
			handled = handledTemp;
			ret = retTemp;
		}
	}

	if (handled)
		return ret;
	else
		return DefWindowProc(hwnd, msg, wParam, lParam);

}

void Quad::BaseWindowController::SetClientMousePosX(int pos)
{
	mClientMousePosX = pos;
}

void Quad::BaseWindowController::SetClientMousePosY(int pos)
{
	mClientMousePosY = pos;
}

int Quad::BaseWindowController::GetClientMousePosX() const
{
	return mClientMousePosX;
}

int Quad::BaseWindowController::GetClientMousePosY() const
{
	return mClientMousePosY;
}


float Quad::BaseWindowController::GetWindowPositionX() const
{
	return mWindowPositionX;
}

float  Quad::BaseWindowController::GetWindowPositionY() const
{
	return mWindowPositionY;
}

void  Quad::BaseWindowController::SetWindowPositionX(float x)
{
	mWindowPositionX = x;
}

void  Quad::BaseWindowController::SetWindowPositionY(float y)
{
	mWindowPositionY = y;
}

Quad::RenderSystem* Quad::BaseWindowController::GetRenderSystem()
{
	return mRenderSystem;
}

const Quad::RenderSystem* Quad::BaseWindowController::GetRenderSystem() const
{
	return mRenderSystem;
}
