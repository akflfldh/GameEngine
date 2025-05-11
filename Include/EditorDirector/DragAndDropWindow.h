#pragma once


#include"Predeclare.h"
#include"Singleton.h"
#include"BaseWindow.h"
namespace Quad
{
	class DragAndDropWindow:public BaseWindow , public Singleton<DragAndDropWindow>
	{
	public:

		DragAndDropWindow(HINSTANCE hInstance);
		virtual ~DragAndDropWindow() = default;

		void Initialize(DragAndDropWindowController* controller,unsigned short width, unsigned short height);

		virtual void Update(float deltaTime) override;
		virtual void UploadObjectToRenderSystem()override;
		void EndUpdate(float deltaTime)override;
		virtual void Draw()override;
		void CreateWindowClass();


		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		//static void SetWindowShowState(bool state);
		//static bool GetWindowShowState();
	private:
		LRESULT CALLBACK InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
		DragAndDropWindowController* mWindowController;


		//bool mWindowShowState;
	};


}