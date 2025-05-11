#pragma once
#include"header.h"
#include"Predeclare.h"
#include"CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB BaseWindowMsgHandlerComponent
	{
	public:
		BaseWindowMsgHandlerComponent() = default;
		virtual ~BaseWindowMsgHandlerComponent() = default;
		
		virtual LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret) = 0;

	private:


	};


}
