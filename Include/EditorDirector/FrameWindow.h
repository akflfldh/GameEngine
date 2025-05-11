#pragma once


#include"Predeclare.h"

#include"EditorDirector/Window.h"
#include"Utility/Singleton.h"
namespace Quad
{

	class FrameWindow:public TaskWindow,public Singleton<FrameWindow>
	{
	public:
		FrameWindow(HINSTANCE hInstance);
		virtual ~FrameWindow() = default;



		virtual void CreateWindowClass() override;
		virtual void Initialize(BaseWindowController * windowController) override;

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		bool GetProjectSelectSceneFlag() const;
		void SetProjectSelectSceneFlag(bool flag);
	private:


		//프로젝트선택장면상태인지여부
		bool mProjectSelectSceneFlag;

		//이런타이틀바 유무 타이틀바의 크기, 너비 등등 이런것들을 window클래스에서 컨트롤러에게 넘겨서 컨트롤러가
		//frame system에다가 타이틀바를 그런한크기로 만드는거지.
		//그리고 child window크기  레이아웃도 초기화하는거지.



	};

}

