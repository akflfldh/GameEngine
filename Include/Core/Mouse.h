#pragma once


#include"Predeclare.h"
#include"Singleton.h"
#include<Windows.h>
#include"Core/CoreDllExport.h"
namespace Quad
{
	//물론 이벤트로도 정보가 전달되는데 , 그러면 쓸모가없는것인가?
	//근데 이렇게하면 엔티티들마다마다 마우스정보를 자체적으로 유지해야할필요가없어지는거지.
	//버튼이 눌려져있다 true이다라는정보를 유지할필요가없다.
	//이말이야.
	class InputSystem;

	class CORE_API_LIB Mouse :public Singleton<Mouse>
	{

	public:
		Mouse();
		~Mouse() = default;

		void Initlaize(HINSTANCE hInstance);

		static bool GetLButtonState();
		static bool GetRButtonState();
		
		static void SetLButtonState(bool state);
		static void SetRButtonState(bool state);
		
		static void SetVerticalArrowCursor();
		static void SetHorizontalArrowCursor();
		static void SetDefaultCursor();

		static void CaptureObject(Object* object);
		static void ReleaseCaptureObject(Object * object);
		static Object* GetCapturedObject();

		static int GetArrowCursorState();


		static void SetCurrScreenPos(int x, int y);
		static int GetScreenPosDeltaX();
		static int GetScreenPosDeltaY();


		void EndUpdate();

	private:
		//마우스의 현재상태정보등을 저장.
		static bool mLButtonState;
		static bool mRButtonState;
		HINSTANCE mHInstance;


		HCURSOR mDefaultCursor;
		HCURSOR mVerticalSizeCursor;
		HCURSOR mHorizontalSizeCursor;


		Object* mCaptureObject;	//이object만 장치의 이벤트를 받는다.(다른 윈도우, 같은윈도우의 다른시스템, 같은시스템의 다른 엔티티들 모두 
		//이벤트를 받지못한다.


		int mArrowCursorState = 0;	//1 hor ,2 ver



		int mCurrScreenPosX;
		int mCurrScreenPosY;

		int mBeforeScreenPosX;
		int mBeforeScreenPosY;

	};

}
