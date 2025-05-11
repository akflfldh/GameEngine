#pragma once

#include"EditorDirector/Window.h"
#include"Utility/Singleton.h"

namespace Quad
{
	class FileUiWindow :public TaskWindow,public Singleton<FileUiWindow>
	{
	public:
		FileUiWindow(HINSTANCE hInstance);
		~FileUiWindow() = default;

		virtual void Initialize(BaseWindowController* windowController);
		virtual void CreateWindowClass();


		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:

	};

}