#pragma once
#include"Predeclare.h"
#include"BaseWindowMsgHandlerComponent.h"

namespace Quad
{
	class DropFileHandlerComponent:public BaseWindowMsgHandlerComponent
	{
	public:
		DropFileHandlerComponent(DockingWindowController * controller);
		~DropFileHandlerComponent() = default;

		LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret) override final;

	private:
		DockingWindowController* mController;
	};

}
