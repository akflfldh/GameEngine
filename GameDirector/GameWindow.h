#pragma once



#include"BaseWindow.h"
#include"Utility/Singleton.h"

namespace Quad
{
	class GameWindowController;
	class GameWindow :public Quad::BaseWindow, public Quad::Singleton<GameWindow>
	{
	public:
		GameWindow(HINSTANCE hInstance);
		virtual ~GameWindow();
		//다른 창들은 항상 playmode true인것 //항상작동하니 
		//gamePlayWindow는 에디터모드가있고 ,게임플레이모드 두가지모드사이를 전환할수있다.

		virtual void Initialize(Quad::BaseWindowController* windowController) override;
		virtual void Update(float deltaTime) override;
		virtual void UploadObjectToRenderSystem()override;
		virtual void EndUpdate()override;
		virtual void Draw() override;

		virtual void CreateWindowClass() override;

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		virtual LRESULT CALLBACK InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
		GameWindowController* mGameWindowController;
		

	};

}