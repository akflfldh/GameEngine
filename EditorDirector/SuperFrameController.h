#pragma once


#include"SuperController.h"

#include<Windows.h>
namespace Quad
{
	class BaseWindow;

	class SuperFrameController:public SuperController
	{
	public:
		static SuperFrameController* GetInstance();

		virtual ~SuperFrameController();

		void Initialize() override;
		virtual BaseWindow* GetWindow() override;
	

		void TestLButtonDownCallback();

	private:

        LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

       
	private:
		SuperFrameController() = default;
		BaseWindow* mWindow;

	};

}
