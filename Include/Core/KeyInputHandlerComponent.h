#pragma once

#include"BaseWindowMsgHandlerComponent.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB KeyInputHandlerComponent:public BaseWindowMsgHandlerComponent
	{
	public:
		KeyInputHandlerComponent(Controller* controller);
		~KeyInputHandlerComponent() = default;


		LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret) override;
	private:
		LRESULT CommonWM_KEYDOWN(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT CommonWM_KEYUP(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT CommonWM_CHAR(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT CommonWM_IME_COMPOSITION(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



		Controller* mController;

	};

}
