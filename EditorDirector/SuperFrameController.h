#pragma once


#include"SuperController.h"

#include<Windows.h>

namespace UI { class UISystem; }


namespace Quad
{
	class BaseWindow;
	

	class SuperFrameController:public SuperController
	{
	public:
		static SuperFrameController* GetInstance();

		virtual ~SuperFrameController();

		void Initialize() override;

		virtual void PreUpdate() override;

		virtual void Update(float deltaTime) override;
		
		virtual void EndUpdate() override;

		virtual void Draw() override;



		virtual BaseWindow* GetWindow() override;
	

		void TestLButtonDownCallback();

	private:

        LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

       
	private:
		SuperFrameController() = default;
		BaseWindow* mWindow;


		UI::UISystem* mUISystem;
	};

}
