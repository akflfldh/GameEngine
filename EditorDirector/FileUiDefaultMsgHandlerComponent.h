#pragma once



#include"BaseWindowMsgHandlerComponent.h"
namespace Quad
{

	class FileUiWindowController;

	class FileUiDefaultMsgHandlerComponent:public BaseWindowMsgHandlerComponent
	{
	public:
		FileUiDefaultMsgHandlerComponent(FileUiWindowContoller* mController);
		~FileUiDefaultMsgHandlerComponent()=default;


		virtual LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret) override;
	private:



		FileUiWindowContoller* mController;
	};


}