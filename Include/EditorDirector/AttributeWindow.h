#pragma once

#include"Predeclare.h"
#include"header.h"
#include"Window.h"
#include"Utility/Singleton.h"

namespace Quad
{

	class AttributeWindow :public TaskWindow ,public Singleton<AttributeWindow>
	{
	public:

		AttributeWindow(HINSTANCE hInstance);
		~AttributeWindow() = default;

		void Initialize(BaseWindowController* windowController) override;

		virtual void CreateWindowClass() override;

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	/*	LRESULT CALLBACK InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);*/

		//virtual void OnResize();
	private:
		void CreateDefaultPage();



	private:

		//WindowController* mAttributeController;
		//RenderSystem* mRenderSystem;	
		//UiSystem* mUiSystem;			//속성창은 UI만있으면되니깐
		//Quad::Map* mMap;		//이부분이 아마 필요없을꺼다




		KeyBoard* mKeyBoard;
		UINT mClientMousePosX = 0;
		UINT mClientMousePosY = 0;

	};


}

