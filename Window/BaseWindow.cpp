#include "Window/BaseWindow.h"
#include <Window/IWindowEventHandler.h>

Quad::BaseWindow::BaseWindow(HINSTANCE hInstance)
    : mHInstance(hInstance), mClientWidth(600), mClientHeight(800), mMaxClientWidth(1200), mMaxClientHeight(1200),
      mMinClientWidth(200), mMinClientHeight(200), mIWindowEventHandler(nullptr)
{
}

Quad::BaseWindow::~BaseWindow() {}

void Quad::BaseWindow::Initialize()
{
    RegisterRawInputDevice();
}

LRESULT CALLBACK Quad::BaseWindow::InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_NCCREATE:
    {
        CREATESTRUCT *createStrcut = (CREATESTRUCT *)lParam;
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)createStrcut->lpCreateParams);
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    break;
    }

    Quad::BaseWindow *window = (Quad::BaseWindow *)(GetWindowLongPtrA(hwnd, GWLP_USERDATA));
    if (window)
        return window->WndProc(hwnd, msg, wParam, lParam);
    else
        return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK Quad::BaseWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        TextOutW(hdc, 10, 10, L"Hello, Windows!", 16);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_GETMINMAXINFO:
    {

        MINMAXINFO *minmaxInfo = (MINMAXINFO *)lParam;
        RECT clientMaxRect = {0, 0, mMaxClientWidth, mMaxClientHeight};

        AdjustWindowRect(&clientMaxRect, mWinStyle, false);

        //  minmaxInfo->ptMaxSize.x = clientMaxRect.right;
        // minmaxInfo->ptMaxSize.y = clientMaxRect.bottom;
    }
        return 0;
    case WM_SIZE:
    {

        if (mIWindowEventHandler)
        {

            if (wParam == SIZE_MINIMIZED)
            {
                mIWindowEventHandler->OnWindowMinimized();
            }
            else
            {

                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                mClientWidth = clientRect.right;
                mClientHeight = clientRect.bottom;

                if (mClientWidth == 0)
                {
                    return 0;
                }

                mIWindowEventHandler->OnWindowResize(clientRect.right, clientRect.bottom);
            }
        }
    }
        return 0;
    case WM_CHAR:
    {
        if (mIWindowEventHandler)
        {
            mIWindowEventHandler->OnCharEvent(wParam);
        }
    }
        return 0;
    case WM_INPUT:
    {
        if (mIWindowEventHandler == nullptr)
            break;

        // raw input
        UINT dwSize;

        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
        LPBYTE lpb = new BYTE[dwSize];

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
        {
            OutputDebugString(TEXT("GetRawInputData가 올바른 사이즈를 리턴하지않았다\n"));
        }

        mIWindowEventHandler->OnInput();

        RAWINPUT *raw = (RAWINPUT *)lpb;
        DWORD rawInputType = raw->header.dwType;

        if (rawInputType == RIM_TYPEMOUSE)
        {
            RAWMOUSE &rawMouse = raw->data.mouse;

            USHORT mouseFlags = rawMouse.usFlags;
            // TO DO  : 마우스 위치를 화면에 텍스트 출력
            if (mouseFlags & MOUSE_MOVE_ABSOLUTE)
            {

                // rawMouse.lLastX;
                // rawMouse.lLastY;
            }
            else if (rawMouse.lLastX != 0 || rawMouse.lLastY != 0)
            {

                POINT screenPoint, clientPoint;

                GetCursorPos(&screenPoint);
                mLastClientPos = screenPoint;
                ScreenToClient(hwnd, &mLastClientPos);
                //  mInitialized = true;

                GetCursorPos(&screenPoint);
                // clientPoint = screenPoint;
                // ScreenToClient(hwnd, &clientPoint);

                mIWindowEventHandler->SetMousePos(screenPoint.x, screenPoint.y, mLastClientPos.x, mLastClientPos.y);
                /* mIWindowEventHandler->SetMousePos(screenPoint.x, screenPoint.y, mLastClientPos.x,
                 * mLastClientPos.y);*/
                mIWindowEventHandler->OnMouseMove(rawMouse.lLastX, rawMouse.lLastY);
            }

            EInputState mouseInputState = EInputState::eNone;

            if (rawMouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
            {
                mouseInputState |= EInputState::eMouseLButtonDown;
            }
            else if (rawMouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
            {
                mouseInputState |= EInputState::eMouseLButtonUp;
            }

            if (rawMouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
            {

                mouseInputState |= EInputState ::eMouseRButtonDown;
            }
            else if (rawMouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
            {
                mouseInputState |= EInputState::eMouseRButtonUp;
            }

            if (rawMouse.usButtonFlags & RI_MOUSE_WHEEL)
            {
                // A mouseInputState |= EInputState::eMouseWheel;
                mIWindowEventHandler->OnMouseWheel((short)rawMouse.usButtonData);
            }

            if (mouseInputState != 0)
            {
                mIWindowEventHandler->OnMouseButtonEvent(mouseInputState);
            }
        }
        else if (rawInputType == RIM_TYPEKEYBOARD)
        {
            RAWKEYBOARD &rawKeyboard = raw->data.keyboard;

            bool bKeyUp = rawKeyboard.Flags & RI_KEY_BREAK;

            EInputState inputState;
            if (bKeyUp)
            {
                inputState = EInputState::eKeyUp;
            }
            else
            {
                // key down
                inputState = EInputState::eKeyDown;
            }

            if (rawKeyboard.VKey != 0)
                mIWindowEventHandler->OnKeyEvent(inputState, rawKeyboard.MakeCode); // rawKeyboard.MakeCode
        }

        delete[] lpb;
    }
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

bool Quad::BaseWindow::CreateWindowClass(LPCWSTR windowClassName, LPCWSTR windowName, DWORD windowStyle,
                                         UINT windowClassStyle)
{

    WNDCLASSW wc;
    wc.hInstance = GetHInstance();
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = windowClassName;
    wc.lpfnWndProc = InnerWndProc;
    wc.style = windowClassStyle;

    if (!RegisterClassW(&wc))
    {
        MessageBoxW(0, L"RegisterClass Failed", 0, 0);
        return false;
    }

    RECT windowClientRect{0, 0, (LONG)GetClientWidth(), (LONG)GetClientHeight()};
    int windowWidth = 0;
    int windowHeight = 0;

    mWinStyle = windowStyle;
    if (AdjustWindowRect(&windowClientRect, mWinStyle, false))
    {
        windowWidth = windowClientRect.right - windowClientRect.left;
        windowHeight = windowClientRect.bottom - windowClientRect.top;
    }

    HWND hwnd = CreateWindowW(wc.lpszClassName, windowName, mWinStyle, 0, 0, windowWidth, windowHeight, 0, 0,
                              GetHInstance(), this);

    ////  SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    // HWND hwnd = CreateWindowEx(WS_EX_ACCEPTFILES | WS_EX_LAYERED  /*WS_EX_TOOLWINDOW*/, L"FileUiWindow",
    // L"FileUiWindow", WS_POPUP | WS_MINIMIZEBOX,

    if (hwnd == NULL)
    {
        MessageBoxW(0, L"CreateWindow Failed", 0, 0);
        return false;
        // return false;
    }

    SetWindowHandle(hwnd);
    // GetClientRect(hwnd, &windowClientRect);

    // ShowWindow(hwnd, SW_SHOW);
    //  UpdateWindow(hwnd);

    return true;
}

void Quad::BaseWindow::SetVisible(bool flag)
{
    if (flag)
    {
        ShowWindow(mWindowHandle, SW_SHOW);
    }
    else
    {
        ShowWindow(mWindowHandle, SW_HIDE);
    }

    UpdateWindow(mWindowHandle);
}

void Quad::BaseWindow::SetIWindowEventHandler(IWindowEventHandler *windowEventHandler)
{

    mIWindowEventHandler = windowEventHandler;
}

void Quad::BaseWindow::Show(UINT showCmd)
{
    ShowWindow(mWindowHandle, showCmd);
    UpdateWindow(mWindowHandle);
}

HINSTANCE Quad::BaseWindow::GetHInstance() const
{
    return mHInstance;
}

HWND Quad::BaseWindow::GetWindowHandle() const
{
    return mWindowHandle;
}

void Quad::BaseWindow::SetWindowHandle(HWND handle)
{
    mWindowHandle = handle;
}

void Quad::BaseWindow::SetClientWidth(unsigned short width)
{
    unsigned short maxClientWidth = GetMaxClientWidth();
    unsigned short minClientWidth = GetMinClientWidth();

    if (width > maxClientWidth)
        width = maxClientWidth;
    else if (width < minClientWidth)
        width = minClientWidth;

    mClientWidth = width;
}

void Quad::BaseWindow::SetClientHeight(unsigned short height)
{
    unsigned short maxClientHeight = GetMaxClientHeight();
    unsigned short minClientHeight = GetMinClientHeight();

    if (height > maxClientHeight)
        height = maxClientHeight;
    else if (height < minClientHeight)
        height = minClientHeight;

    mClientHeight = height;
}

unsigned short Quad::BaseWindow::GetClientWidth() const
{
    return mClientWidth;
}

unsigned short Quad::BaseWindow::GetClientHeight() const
{
    return mClientHeight;
}

void Quad::BaseWindow::SetMaxClientWidth(unsigned short width)
{
    mMaxClientWidth = width;
}

void Quad::BaseWindow::SetMaxClientHeight(unsigned short height)
{
    mMaxClientHeight = height;
}

unsigned short Quad::BaseWindow::GetMaxClientWidth() const
{
    return mMaxClientWidth;
}

unsigned short Quad::BaseWindow::GetMaxClientHeight() const
{
    return mMaxClientHeight;
}

void Quad::BaseWindow::SetMinClientWidth(unsigned short width)
{
    mMinClientWidth = width;
}

void Quad::BaseWindow::SetMinClientHeight(unsigned short height)
{
    mMinClientHeight = height;
}

unsigned short Quad::BaseWindow::GetMinClientWidth() const
{
    return mMinClientWidth;
}

unsigned short Quad::BaseWindow::GetMinClientHeight() const
{
    return mMinClientHeight;
}

void Quad::BaseWindow::SetMouseCapture(bool flag)
{

    if (flag)
        SetCapture(mWindowHandle);
    else
        ReleaseCapture();
}

void Quad::BaseWindow::SetKeyboardCapture(bool flag)
{

    if (flag)
    {
        SetFocus(mWindowHandle);
    }
    else
    {
        // Releasing the app-level keyboard capture should not drop native window focus.
        // Clearing focus here causes Win32 to emit the default beep after finishing text input
        // because subsequent key input no longer has a focused target window.
    }
}

void Quad::BaseWindow::SetCursorVisible(bool visible)
{
    // ShowCursor의 표시 카운터는 HWND별 상태가 아니라 스레드 단위 상태다.
    // 같은 스레드의 여러 창이 상태를 공유해야 창 전환 후 복원 요청이 누락되지 않는다.
    static thread_local bool hasAppliedVisibility = false;
    static thread_local bool appliedVisibility = true;
    if (hasAppliedVisibility && appliedVisibility == visible)
        return;

    // 단순 bool 대입 API가 아니므로 표시(0 이상) / 숨김(음수) 경계까지 맞춘다.
    // 동일 상태의 반복 요청은 위에서 차단하여 표시 카운터가 계속 누적되지 않게 한다.
    if (visible)
    {
        while (::ShowCursor(TRUE) < 0)
        {
        }
    }
    else
    {
        while (::ShowCursor(FALSE) >= 0)
        {
        }
    }

    appliedVisibility = visible;
    hasAppliedVisibility = true;
}

bool Quad::BaseWindow::ClipCursor(const RECT *clientRect)
{
    // 커서 제한은 HWND별 상태가 아니다. 해제에는 좌표 변환이나 유효한 창 핸들이 필요 없다.
    if (clientRect == nullptr)
        return ::ClipCursor(nullptr) != FALSE;

    if (mWindowHandle == nullptr || clientRect->right <= clientRect->left || clientRect->bottom <= clientRect->top)
        return false;

    // 전달받은 영역에는 논리적 창의 오프셋이 이미 포함되어 있어야 한다.
    // Win32 ClipCursor는 화면 좌표를 요구하므로 클라이언트 원점을 기준으로 두 모서리를 변환한다.
    POINT topLeft = {clientRect->left, clientRect->top};
    POINT bottomRight = {clientRect->right, clientRect->bottom};
    if (!::ClientToScreen(mWindowHandle, &topLeft) || !::ClientToScreen(mWindowHandle, &bottomRight))
        return false;

    RECT screenRect = {topLeft.x, topLeft.y, bottomRight.x, bottomRight.y};
    return ::ClipCursor(&screenRect) != FALSE;
}

void Quad::BaseWindow::ShutDown()
{
    if (mWindowHandle)
    {
        PostQuitMessage(0);
    }
}

bool Quad::BaseWindow::RegisterRawInputDevice()
{
    RAWINPUTDEVICE Rid[2]; // mouse , keyboard

    // mouse
    Rid[0].usUsagePage = 0x0001;
    Rid[0].usUsage = 0x0002;
    Rid[0].dwFlags = 0;
    Rid[0].hwndTarget = 0;

    // keyboard
    Rid[1].usUsagePage = 0x0001;
    Rid[1].usUsage = 0x0006;
    Rid[1].dwFlags = 0;
    Rid[1].hwndTarget = 0;

    if (!RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])))
    {

        // error;
        return false;
    }

    return true;
}
