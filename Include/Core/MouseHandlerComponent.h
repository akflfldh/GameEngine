#pragma once

#include"BaseWindowMsgHandlerComponent.h"
#include"CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB MouseHandlerComponent:public BaseWindowMsgHandlerComponent
	{
	public:
		MouseHandlerComponent(BaseWindowController* controller);
		virtual ~MouseHandlerComponent()=default;


		LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret);

	private:
		BaseWindowController* mController;


	};

}
