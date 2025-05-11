#pragma once
#include"Predeclare.h"
#include"BaseWindow.h"
#include"Utility/Singleton.h"

namespace Quad
{

	class PopupWindow:public BaseWindow, public Singleton<PopupWindow>
	{
	public:
		PopupWindow(HINSTANCE hInstance, int clientWidth, int clientHeight,bool visibleFlag);
		~PopupWindow()=default;

		void Initialize(BaseWindowController* controller);

		virtual void Update(float deltaTime)override;
		virtual void UploadObjectToRenderSystem()override;
		virtual void EndUpdate(float deltaTime)override;
		virtual void Draw() override;
		virtual void CreateWindowClass()override;


		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


		static void SetVisibilityState(bool flag); 
	//	static void ActiveWindow(int screenPosx, int screenPosY,bool visibleFlag);

		bool GetVisibilityState() const;

	protected:

		//void AdjustPosition(int screenPosX, int screenPosY);
		//HMONITOR GetMinitorHandle() const;

		virtual LRESULT CALLBACK InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


	private:
		bool mVisibleState;//맨초기에는 보이지않을것이다.
		HMONITOR mMonitorHandle;

		PopupWindowController* mPopupController;//baseWindow에있는 windowController를 캐싱
		






	
	};

}
