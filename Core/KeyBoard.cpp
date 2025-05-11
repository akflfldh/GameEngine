#include "Core/KeyBoard.h"
#include"Core/InputSystem.h"

#include"TextBox.h"

namespace Quad
{




	HHOOK KeyBoard::g_hook = NULL;


	bool KeyBoard::mTextBoxActiveFlag;
	 std::unordered_map<VK_KEY, KEY_DOWN_FLAG> KeyBoard::mVKTable;


	 KeyBoard::KeyBoard()
	 {
		


	 }
	 void Quad::KeyBoard::Initialize()
	{
		for (wchar_t i = 0; i < 256; ++i)
		{
			mVKTable[i] = KEY_UP;
		}

		mCapslockFlag = false;
		mHangulCompositionStartFlag = false;
		mHangulCompositionContinueFlag = false;
		if ((GetAsyncKeyState(VK_CAPITAL) & 0x0001) != 0)
			mCapslockFlag = true;

		mTextBoxActiveFlag = false;
		mLanguageState = 0;


		ActiveSystemHook();






	}

	 void KeyBoard::SetHwnd(HWND hwnd)
	 {
		 auto instance = GetInstance();
		 instance-> mHIMCperHwndTable.insert({ hwnd,ImmGetContext(hwnd) });
		
	 }

	 void KeyBoard::KeyDown(VK_KEY vk_key)
	{
		mVKTable[vk_key] = KEY_DOWN;
		auto inputSystem =	 InputSystem::GetInstance();
		inputSystem->OnEvent(vk_key, Quad::EInputState::eKeyDown);


	/*	if (vk_key == VK_CAPITAL)
		{
			mCapslockFlag = mCapslockFlag ? false : true;
		}
		else if (vk_key == VK_HANGUL)
		{
			mHangulFlag = mHangulFlag ? false : true;
		}*/

	}

	void KeyBoard::KeyUp(VK_KEY vk_key)
	{
		mVKTable[vk_key] = KEY_UP;
		auto inputSystem = InputSystem::GetInstance();
		inputSystem->OnEvent(vk_key, Quad::EInputState::eKeyUp);



	}

	bool KeyBoard::IsKeyDown(VK_KEY vk_key)
	{
		return mVKTable[vk_key];
	}


	UNICODE_VALUE Quad::KeyBoard::GetCurrentCharacterKey(VK_KEY vk_key)
	{

		return 0;
	}

	void KeyBoard::SetTextBoxActiveFlag(bool flag, HWND hwnd ,Object * object)
	{
		auto instance = GetInstance();
		DWORD threadID = GetWindowThreadProcessId(hwnd,nullptr);
		mTextBoxActiveFlag = flag;

		//활성화시키는것이라면 다른 엔티티가 소유하더라도 뻇으면된다.

		TextBox* textBox = static_cast<TextBox*>(object);




		if (mTextBoxActiveFlag == true)
		{
			auto himcIt = instance->mHIMCperHwndTable.find(hwnd);
			HIMC hIMC = himcIt->second;


			instance->mBoxActiveObject = object;

			//HIMC hIMC = ImmGetContext(hwnd);
			DWORD currentCMODE, sentence;
			ImmGetConversionStatus(hIMC, &currentCMODE, &sentence);
			if (textBox->GetOnlyEnglishFlag())
			{
				ImmAssociateContext(hwnd, NULL);

			}
			else
			{
				
				ImmSetConversionStatus(hIMC, IME_CMODE_NATIVE, sentence);
				ImmSetOpenStatus(hIMC, true);
				ImmAssociateContext(hwnd,hIMC);
			}

			
			ImmReleaseContext(hwnd, hIMC);

			if (g_hook)
			{
				UnhookWindowsHookEx(g_hook);
				g_hook = NULL;
			}



		}
		else
		{
			//비활성화하는것이라면 이미 다른누군가가 활성화해서 소유권을 가져갔는지 본다.
			//가져갔다면 다음처리는 하지않는다.
			if (object != instance->mBoxActiveObject)
				return;

			auto himcIt = instance->mHIMCperHwndTable.find(hwnd);
			HIMC hIMC = himcIt->second;

			//영어로 전환  //단 alt값은 여전히 유지되어 텍스트모드가 활성화되면 그 언어모드로 전환할때 사용
			//HIMC hIMC = ImmGetContext(hwnd);
			DWORD currentCMODE, sentence;

			ImmGetConversionStatus(hIMC, &currentCMODE, &sentence);
			ImmSetConversionStatus(hIMC, IME_CMODE_CHARCODE, sentence);

			ImmSetOpenStatus(hIMC, false);

			ImmReleaseContext(hwnd, hIMC);




			if(g_hook ==NULL)
			g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL),
				0);



		}




	}

	bool KeyBoard::GetTextBoxActiveFlag()
	{
		return mTextBoxActiveFlag;
	}

	Object* KeyBoard::GetCapturedObject()
	{
		auto instance = GetInstance();
		return instance->mCaptureObject;
	}

	void KeyBoard::CaptureObject(Object* object)
	{
		auto instance = GetInstance();


		

		instance->mCaptureObject = object;


	}

	void KeyBoard::ReleaseCaptureObject(Object * object)
	{
		auto instance = GetInstance();

		//소유권을 다른누군가가 가져갔다면 처리하지않는다.
		if (instance->mCaptureObject != object)
			return;

		instance->mCaptureObject = nullptr;
		

	}

	void KeyBoard::SetHangulCompositionStartFlag(bool flag)
	{
		auto instance = GetInstance();
		instance->mHangulCompositionStartFlag = flag;
	}

	bool KeyBoard::GetHangulCompositionStartFlag()
	{
		auto instance = GetInstance();
		return instance->mHangulCompositionStartFlag;
	}

	void KeyBoard::SetHangulCompositionContinueFlag(bool flag)
	{
		auto instance = GetInstance();
		instance->mHangulCompositionContinueFlag = flag;
	}

	bool KeyBoard::GetHangulCompositionContinueFlag()
	{

		auto instance = GetInstance();
		return instance->mHangulCompositionContinueFlag;
	}

	LRESULT KeyBoard::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		

		if (nCode == HC_ACTION)
		{
			PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

			// 한/영 키(VK_HANGUL) 감지 및 차단
			if (p->vkCode == VK_HANGUL || p->vkCode == VK_HANGEUL)
			{
				return 1; // 키 이벤트 차단
			}
		}
		return CallNextHookEx(g_hook, nCode, wParam, lParam);
	}

	void KeyBoard::ActiveSystemHook()
	{
		if (g_hook == NULL)
			g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL),
				0);
	}

	void KeyBoard::InActiveSystemHook()
	{

		if (g_hook)
		{
			UnhookWindowsHookEx(g_hook);
			g_hook = NULL;
		}



	}

	int KeyBoard::GetUpperCaseFlag()
	{
		auto instance = GetInstance();
		TextBox* textBox = static_cast<TextBox*>(instance->mCaptureObject);

		if (textBox)
		{
			return textBox->GetOnlyUpperCaseFlag();
		}


		return false;
		
	}

	





}