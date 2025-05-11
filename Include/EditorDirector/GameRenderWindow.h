#pragma once


#include"Window.h"
#include"Singleton.h"
#include"RenderSystem.h"
#include"System/GamePlayUiSystem.h"
#include"GamePlayWindowController.h"

namespace Quad
{
	class GameRenderWindow :public TaskWindow, public Singleton<GameRenderWindow>
	{
	public:
		GameRenderWindow(HINSTANCE hInstance);
		~GameRenderWindow() = default;

		//초기화할때 여러가지 window 옵션을 설정한다.
		virtual void Initialize(BaseWindowController* windowController);

		virtual void CreateWindowClass() override;
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		


	private:






	};

}