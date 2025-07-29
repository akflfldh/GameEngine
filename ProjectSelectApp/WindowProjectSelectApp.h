#pragma once



#ifdef _WINDOWS
 
#include<Windows.h>
#include<BaseWindow.h>

#include"ProjectConfig.h"
#include<vector>
#include<JsonParser.h>

namespace QuadPSA
{
	class WindowProjectSelectApp
	{
	public:
		static WindowProjectSelectApp* GetInstance();
		WindowProjectSelectApp(HINSTANCE hInstance);
		virtual ~WindowProjectSelectApp();

		static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


		void Initialize();

		void CreateWindowController(HWND hwnd);
		void Run();

		void EndApp();

	private:
		LRESULT InnerWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void EexcuteProject();



	private:
		static WindowProjectSelectApp* mInstance;
		
		JsonParser::Parser mJsonParser;
		Quad::BaseWindow mWindow;



		HWND mProjectExecuteButtonHandle;
		HWND mProjectListHandle;


		 






		std::vector< ProjectConfig> mProjectConfigLists;
		bool mRunFlag;
		int mSelectedProjectIndex;

	};

}


#endif