#pragma once

#include"BaseWindowMsgHandlerComponent.h"

namespace Quad
{
	class PopupWindowDefulatMsgHandlerComponent:public BaseWindowMsgHandlerComponent
	{
	public:
		PopupWindowDefulatMsgHandlerComponent(PopupWindowController* controller);
		~PopupWindowDefulatMsgHandlerComponent() = default;

		LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret) override;

	private:
		PopupWindowController* mController;
		
	};

}
