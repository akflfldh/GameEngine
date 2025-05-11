#include "KeyInputHandlerComponent.h"
#include"KeyBoard.h"
#include"KeyEvent.h"
#include"Controller.h"
#include"WindowActiveEvent.h"

Quad::KeyInputHandlerComponent::KeyInputHandlerComponent(Controller* controller)
	:mController(controller)
{
}

LRESULT Quad::KeyInputHandlerComponent::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret)
{

   
    
    switch (msg)
    {
    case WM_ACTIVATE:
    {
        ret = true;
        WindowActiveEvent* activeEvent = new WindowActiveEvent;
        //여기서는 키보드의 키보드에 관한것만처리하고 나머지 주요active에 관해서처리는다른컴포넌트에서 처리할것이다
        switch (wParam)
        {
        case WA_CLICKACTIVE:
        case WA_ACTIVE:
            KeyBoard::ActiveSystemHook();
            activeEvent->SetWindowActiveState(1);
            break;
        case WA_INACTIVE:
        {
            KeyBoard::InActiveSystemHook();
            activeEvent->SetWindowActiveState(0);



        }

        }

        if (!KeyBoard::GetTextBoxActiveFlag())
        {

            HIMC hIMC = ImmGetContext(hwnd);
            DWORD currentCMODE, sentence;

            ImmGetConversionStatus(hIMC, &currentCMODE, &sentence);
            ImmSetConversionStatus(hIMC, IME_CMODE_CHARCODE, sentence);
            ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0); // 조합 중인 상태를 취소
            ImmSetOpenStatus(hIMC, FALSE);

        }
            // ImmReleaseContext(hwnd, hIMC);
        mController->AddEvent(activeEvent);
        return 0;
    }
    break;

    case WM_KEYDOWN:
        ret = true;
        return CommonWM_KEYDOWN(hwnd, msg, wParam, lParam);

    case WM_KEYUP:
        ret = true;
        return CommonWM_KEYUP(hwnd, msg, wParam, lParam);
    case WM_CHAR:

    {

        ret = true;
        if (wParam == VK_HANGUL || wParam == VK_KANA || wParam == VK_MENU)
        {
            // 한/영 키 입력을 가로채어 IME 상태 변경을 방지
            return 0; // 메시지를 처리하고 추가로 전달하지 않음
        }

        return CommonWM_CHAR(hwnd, msg, wParam, lParam);


    }
    //case WM_IME_CHAR:

    //{
    //    wchar_t te = wParam;
    //    int a = 2;
    //    //이거는 wm_ime_composition에서 보내는데 우리는 composition에서 처리를하고 default보내고
    //    //이메세지는 처리하면안되는데 문제는 char로 wm_char로 보낸다는거야 여기서 또 default로보내면
    //    //그러니 여기서는 보내지않도록해야한다.
    //}
    //return 0;       //그래서 그냥 return 0이다 또생각해보면 compositon에서도 그냥 return 0을해도될거같고
       //return DefWindowProc(hwnd, msg, wParam, lParam);
    case WM_IME_COMPOSITION:

        ret = true;
        if (KeyBoard::GetTextBoxActiveFlag())
        {
            return  CommonWM_IME_COMPOSITION(hwnd, msg, wParam, lParam);
        }
        else
        {

            int a = 2;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);

    case WM_SYSKEYDOWN:
    {
        ret = true;
        if (wParam == VK_HANGUL || wParam == VK_KANA || wParam == VK_RMENU || wParam == VK_MENU)
        {
            // 한/영 키 입력을 가로채어 IME 상태 변경을 방지
            return 0; // 메시지를 처리하고 추가로 전달하지 않음
        }
        // 다른 키 입력 처리
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    case WM_IME_KEYDOWN:
    {
        ret = true;
        if (wParam == VK_HANGUL || wParam == VK_KANA || wParam == VK_RMENU || wParam == VK_MENU)
        {
            // 한/영 키 입력을 가로채어 IME 상태 변경을 방지
            return 0; // 메시지를 처리하고 추가로 전달하지 않음
        }
        // 다른 키 입력 처리
        return DefWindowProc(hwnd, msg, wParam, lParam);

    }
    case WM_SYSKEYUP:
    {
        ret = true;
        if (wParam == VK_HANGUL || wParam == VK_KANA || wParam == VK_RMENU || wParam == VK_MENU)
        {
            // 한/영 키 릴리스도 가로채기
            return 0;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }


    }

  
    return 0;

}

LRESULT Quad::KeyInputHandlerComponent::CommonWM_KEYDOWN(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    VK_KEY key = wParam;

    if (wParam == VK_HANGUL || wParam == VK_KANA || wParam == VK_RMENU || wParam == VK_MENU)
    {
        // 한/영 키 입력을 가로채어 IME 상태 변경을 방지
        int a = 2;
        return 0; // 메시지를 처리하고 추가로 전달하지 않음
    }
    if (KeyBoard::GetTextBoxActiveFlag())
    {


        //텍스박스가 받는 유일한 가상키는 화살표방향키
        if (key == VK_LEFT || key == VK_RIGHT || key == VK_UP || key == VK_DOWN || key == VK_BACK)
        {


            KeyEvent* event = new KeyEvent("KeyDown");
            KeyBoard::KeyDown(key);
            event->SetKey(key);     //숫자는 가상키
            event->SetVKFlag(true);
            mController->AddEvent(event);
        }
    }
    else
    {

        //HIMC hIMC = ImmGetContext(hwnd);
        //DWORD currentCMODE, sentence;

        //ImmGetConversionStatus(hIMC, &currentCMODE, &sentence);
        //ImmSetConversionStatus(hIMC, IME_CMODE_CHARCODE, sentence);
        //ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0); // 조합 중인 상태를 취소
        //ImmSetOpenStatus(hIMC, FALSE);

        KeyEvent* event = new KeyEvent("KeyDown");
        KeyBoard::KeyDown(key);
        event->SetKey(key);
        event->SetVKFlag(true);
        mController->AddEvent(event);

    }

    //return DefWindowProc(hwnd, msg, wParam, lParam);
    return 0;
}

LRESULT Quad::KeyInputHandlerComponent::CommonWM_KEYUP(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!KeyBoard::GetTextBoxActiveFlag())
    {
        KeyEvent* event = new KeyEvent("KeyUp");
        KeyBoard::KeyUp(wParam);
        event->SetKey(wParam);
        mController->AddEvent(event);
    }

    return  DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Quad::KeyInputHandlerComponent::CommonWM_CHAR(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (KeyBoard::GetTextBoxActiveFlag())
    {
        KeyEvent* event = new KeyEvent("KeyDown");

        if (wParam != VK_BACK)
        {
            //mKeyBoard->KeyDown(wParam);


            bool upperCaseFlag  =  KeyBoard::GetUpperCaseFlag();
            if (upperCaseFlag)
            {
                if (std::islower(wParam))
                {
                    wParam = std::toupper(wParam);
                }
            }



            event->SetCharacter(wParam);
            event->SetVKFlag(false);

            mController->AddEvent(event);
        }
    }
    //return DefWindowProc(hwnd, msg, wParam, lParam);
    return 0;
}

LRESULT Quad::KeyInputHandlerComponent::CommonWM_IME_COMPOSITION(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //한글에서 백스페이스프를 담은 메세지는 진행중인문자에서는 발생하지않는다.
    HIMC hIMC = ImmGetContext(hwnd);
    wchar_t str;
    uint32_t len;
    if (lParam & GCS_RESULTSTR) {
        len = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);
        ImmGetCompositionString(hIMC, GCS_RESULTSTR, &str, len);

        // 완성된 글자
        OutputDebugString(L"CharDown5\n");
        KeyEvent* event = new KeyEvent("KeyDown");

        KeyBoard::KeyDown(wParam);
        KeyBoard::SetHangulCompositionStartFlag(false);
        KeyBoard::SetHangulCompositionContinueFlag(false);
        //event->SetKey(wParam);
        event->SetCharacter(str);
        event->SetHangulFlag(true);
        event->SetHangulCompositionFlag(false);
        event->SetVKFlag(false);
        mController->AddEvent(event);

    }

    if (lParam & GCS_COMPSTR) {
        len = ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0);
        if (len != 0)
        {   //backspace를 눌러 조합중인문자가 완전히 사라지면 0 , 그떈 wm_keydown메세지에서 backspace keydown이벤트륿 보낼것이다.
            ImmGetCompositionString(hIMC, GCS_COMPSTR, &str, len);


            // 조합중인 글자 
            //완성된문자가 아니라 조합중인 문자라는것을알려한다.
            OutputDebugString(L"CharDown6\n");
            // backspace를 눌렀을떄 esc 마지막구성문자를 지웠을떄 발생한다.
            // 근데 backspace는 한글조합중에서는 전달되지는않는다.
            if ((wchar_t)wParam == 27)
            {
                str = wParam;
            }
            KeyEvent* event = new KeyEvent("KeyDown");
            KeyBoard::KeyDown(wParam);



            if (KeyBoard::GetHangulCompositionStartFlag() == false && KeyBoard::GetHangulCompositionContinueFlag() == false)
            {
                KeyBoard::SetHangulCompositionStartFlag(true);//조합 첫시작

            }
            else
            {
                KeyBoard::SetHangulCompositionStartFlag(false);
                KeyBoard::SetHangulCompositionContinueFlag(true);
            }



            //event->SetKey(wParam);
            event->SetCharacter(str);
            event->SetHangulFlag(true);
            event->SetHangulCompositionFlag(true);
            event->SetVKFlag(false);
            mController->AddEvent(event);
        }
        else
        {
            KeyEvent* event = new KeyEvent("KeyDown");
            KeyBoard::SetHangulCompositionStartFlag(false);
            KeyBoard::SetHangulCompositionContinueFlag(false);
            KeyBoard::KeyDown(VK_BACK);
            event->SetKey(VK_BACK);     //숫자는 가상키
            event->SetVKFlag(true);
            mController->AddEvent(event);
        }
    }

    if (len == 0)
        return DefWindowProc(hwnd, msg, wParam, lParam);
    ImmReleaseContext(hwnd, hIMC);

    return 0;
}
