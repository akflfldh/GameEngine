#pragma once

#include"Predeclare.h"
#include"header.h"
#include"Utility/Singleton.h"
#include"Core/CoreDllExport.h"
#include<unordered_map>


namespace Quad
{

	class CORE_API_LIB KeyBoard:public Singleton<KeyBoard>
	{
	public:
		KeyBoard();
		~KeyBoard() = default;

		void Initialize();


		//keyboard를 사용할 창이라면 등록
		static void SetHwnd(HWND hwnd);

		static void KeyDown(VK_KEY vk_key);
		static void KeyUp(VK_KEY vk_key);

		static bool IsKeyDown(VK_KEY vk_key);


		//'자판A'가눌렸어 그럼 현재키보드 상태를 따졌을떄 실제 문자는 어떻게되는가
		//문자가 아닌 VK_KEY는 유니코드 0을 리턴하자.
		UNICODE_VALUE GetCurrentCharacterKey(VK_KEY vk_key);


		//어떤엔티티가 활성화했고 비활성화했는지알아야한다. 그래야 
		//이미 다른엔티티가 활성화시켜서 소유권을 가져갔다면 
		//이후에 비활성화시킬려는 다른엔티티가  비활성화하지못하게 할수있다.
		static void SetTextBoxActiveFlag(bool flag ,HWND hwnd,Object * object);
		static bool GetTextBoxActiveFlag();


		static Object * GetCapturedObject();
		static void CaptureObject(Object* object);
		static void ReleaseCaptureObject(Object* object);

		static void SetHangulCompositionStartFlag(bool flag);
		static bool GetHangulCompositionStartFlag() ;

		static void SetHangulCompositionContinueFlag(bool flag);
		static bool GetHangulCompositionContinueFlag();

		static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
		static void ActiveSystemHook();
		static void InActiveSystemHook();
		
		static int GetUpperCaseFlag();
		

	private:
	

	private:


		//VK_KEY 'A' 와 연관된 UNICODE문자들 'A','a','ㅁ'
		static std::unordered_map<VK_KEY,KEY_DOWN_FLAG> mVKTable;
		//std::unordered_map <VK_KEY, std::vector<std::pair<KEY_DOWN_FLAG, UNICODE_VALUE>>>mCharacterKeyTable;
		bool mCapslockFlag;
		bool mHangulCompositionStartFlag;
		bool mHangulCompositionContinueFlag;


		//올바르지않다
		std::unordered_map<VK_KEY, wchar_t> mHangulTable;
		static bool mTextBoxActiveFlag;


		Object* mCaptureObject = nullptr;
		Object* mBoxActiveObject = nullptr;

		//alt키가 눌렸을때마다 바뀐다, 
		//강제로 시스템함수로 설정하는것은 반영하지않는다
		//텍스트모드로 전환할때 키입력언어를 올바르게복구시키기위한용도
		int mLanguageState;		//0 : 영어(디폴트) //1: 한글
		static HHOOK g_hook;

		std::unordered_map<HWND, HIMC> mHIMCperHwndTable;
	

		



	};




}

