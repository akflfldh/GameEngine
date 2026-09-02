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

        minmaxInfo->ptMaxSize.x = clientMaxRect.right;
        minmaxInfo->ptMaxSize.y = clientMaxRect.bottom;
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
                //A mouseInputState |= EInputState::eMouseWheel;
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

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

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
