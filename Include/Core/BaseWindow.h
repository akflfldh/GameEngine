#pragma once

#include<Windows.h>


#include"Predeclare.h"

#include"Core/CoreDllExport.h"
namespace Quad
{

	enum class EWindowType
	{
		eFrameWindow=0,
		eGamePlayWindow,
		eAttributeWindow,
		eFileUiWindow,
		ePopUpWindow,


	};

	class CORE_API_LIB BaseWindow
	{
	public:

		BaseWindow(HINSTANCE hInstance, EWindowType WindowID);
		virtual ~BaseWindow() = 0;
											//다른 창들은 항상 playmode true인것 //항상작동하니 
											//gamePlayWindow는 에디터모드가있고 ,게임플레이모드 두가지모드사이를 전환할수있다.

		virtual void Initialize(BaseWindowController* windowController);
		virtual void Update(float deltaTime)=0;
		virtual void UploadObjectToRenderSystem() = 0;
		virtual void EndUpdate(float deltaTime) = 0;
		virtual void Draw()=0;


		HINSTANCE GetHInstance()const;
		virtual void CreateWindowClass() = 0;
		HWND GetWindowHandle() const;
		EWindowType GetWndID() const;
		void SetWindowHandle(HWND handle);
		
		void SetClientWidth(unsigned short width);
		void SetClientHeight(unsigned short height);

		unsigned short GetClientWidth()const;
		unsigned short GetClientHeight()const;

		void SetMaxClientWidth(unsigned short width);
		void SetMaxClientHeight(unsigned short height);

		unsigned short GetMaxClientWidth()const;
		unsigned short GetMaxClientHeight()const;

		void SetMinClientWidth(unsigned short width);
		void SetMinClientHeight(unsigned short height);

		unsigned short GetMinClientWidth()const;
		unsigned short GetMinClientHeight()const;



		void HandleDefaultLButtonDown(WPARAM wParam, LPARAM lParam);
		void HandleDefaultRButtonDown(WPARAM wParam, LPARAM lParam);
		void HandleDefaultLButtonUp(WPARAM wParam, LPARAM lParam);
		void HandleDefaultRButtonUp(WPARAM wParam, LPARAM lParam);
		void HandleDefaultMouseMove(WPARAM wParam, LPARAM lParam);

		BaseWindowController* GetWindowController();
		

	protected:

		
		virtual LRESULT CALLBACK InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) =0;

		int mClientMousePosX = 0;
		int mClientMousePosY = 0;

	private:
		HINSTANCE mHInstance;
		HWND mWindowHandle;
		EWindowType mWindowType;

		unsigned short mClientWidth;		
		unsigned short mClientHeight;

		unsigned short mMaxClientWidth;
		unsigned short mMaxClientHeight;

		unsigned short mMinClientWidth;
		unsigned short mMinClientHeight;




		BaseWindowController* mWindowController;

		float mWindowPositionX;
		float mWindowPositionY;



	};
}

