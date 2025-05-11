#pragma once
#include"Predeclare.h"

#include"header.h"
#include"Core/BaseWindow.h"



namespace Quad
{
	

	class TaskWindow:public BaseWindow
	{
		friend class WindowwController;
		friend class FrameWindowController;

	public:
		TaskWindow(HINSTANCE hInstance, EWindowType WindowID);
		virtual ~TaskWindow() =default;

 

		virtual void CreateWindowClass()=0 ;
		virtual void Update(float deltaTime) override;
		virtual void UploadObjectToRenderSystem()override;
		virtual void EndUpdate(float deltaTime) override;
		virtual void Draw() override;
	
		virtual void Initialize(BaseWindowController* windowController) override;


		void SetClientWidthFloat(float width);
		void SetClientHeightFloat(float height);
	//	void SetClientWidth(int width);
		//void SetClientHeight(int height);


		float GetClientWidthFloat()const;
		float GetClientHeightFloat() const;
	//	int GetClientWidth() const;
	//	int GetClientHeight() const;



		void OnResize(int width , int height ,int dragDirection);


		void SetWindowMoveFlag(bool flag);
		bool GetWindowMoveFlag() const;


		virtual LRESULT CALLBACK InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
		
	

		//window option methods


		void SetEdgeDragResizeFlag(bool flag);
		bool GetEdgeDragResizeFlag()const;

		void SetEdgeDragResizableDirection(EWindowEdgeDragResizableDirection eDirection, bool state);
		bool GetEdgeDragResizableDirectionState(EWindowEdgeDragResizableDirection eDirection) const;



		void SetDockingState(bool state);
		bool GetDockingState() const;


		float GetWindowPositionX() const;
		float GetWindowPositionY() const;

		void SetWindowPositionX(float x);
		void SetWindowPositionY(float y);
		
		void SetMaxMinClientSizeFlag(bool flag);
		bool GetMaxMinClientSizeFlag()const;
		

		void SetNotDockingMaxClientWidth(unsigned short width);
		void SetNotDockingMaxClientHeight(unsigned short height);
		void SetNotDockingMinClientWidth(unsigned short width);
		void SetNotDockingMinClientHeight(unsigned short height);
		
		unsigned short GetNotDockingMaxClientWidth() const ;
		unsigned short GetNotDockingMaxClientHeight() const;
		unsigned short GetNotDockingMinClientWidth() const ;
		unsigned short GetNotDockingMinClientHeight() const;

		void SetPlayMode(bool playMode);
		bool GetPlayMode()const;

	protected:


		LRESULT CommonWM_KEYDOWN(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,Controller * controller);
		LRESULT CommonWM_KEYUP(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Controller* controller);
		LRESULT CommonWM_CHAR(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Controller* controller);
		LRESULT CommonWM_IME_COMPOSITION(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Controller* controller);

	private:

		DockingWindowController* GetWindowController() const;

	private:
		//HINSTANCE mInstanceHandle;
		//HWND mWindowHandle;
		//창의 크기가 픽셀단위인데 사이즈조절을할때 비율에 비례해서 증가하다보니 프레임윈도우가
		//예를들어 1픽셀움직이면 docking 된 윈도우가 0.2픽셀움직이어야하는데 정수단위다보니 반영이안되는문제가있어서
		//부동소수,정수 두개를 각각관리
		float mClientWidthFloat;		//드래그로 사이즈를 조절할때 사용
		float mClientHeightFloat;
	//	int mClientWidth;		//일반적인곳에서는 다음을사용
		//int mClientHeight;
		//int  mWindowID;
	
		bool mWindowMoveFlag;



		DockingWindowController* mWindowController;
		KeyBoard* mKeyBoard;
	/*	int mClientMousePosX = 0;
		int mClientMousePosY = 0;*/


		bool mMaxMinClientSizeFlag = false;

		unsigned short mNotDockingMaxClientWidth;
		unsigned short mNotDockingMaxClientHeight;
		unsigned short mNotDockingMinClientWidth;
		unsigned short mNotDockingMinClientHeight;


	private:
		//window option flag

		bool mEdgeDragResizeFlag ;
		bool mDockingState;
		bool mEdgeDragResizableDirection[4];		// 1:left, 2: right,3:top, 4:bottom
		int mEdgeDragDirection = 0;// 0: none , 1:left, 2: right,3:top, 4:bottom

		float mWindowPositionX;
		float mWindowPositionY;



	
	};


}