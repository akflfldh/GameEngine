#pragma once

#include"BaseWindowMsgHandlerComponent.h"

namespace Quad
{
	class TaskWindowCommonHandlerComponent:public BaseWindowMsgHandlerComponent
	{
	public:
		TaskWindowCommonHandlerComponent(DockingWindowController* controller);
		~TaskWindowCommonHandlerComponent() = default;

		LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret) override;



	private:
		DockingWindowController* mController;

	};

}
