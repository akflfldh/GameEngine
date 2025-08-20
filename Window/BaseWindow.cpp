#include "Window/BaseWindow.h"

Quad::BaseWindow::BaseWindow(HINSTANCE hInstance)
    : mHInstance(hInstance), mClientWidth(600), mClientHeight(800), mMaxClientWidth(1200), mMaxClientHeight(1200),
      mMinClientWidth(200), mMinClientHeight(200)
{
}

Quad::BaseWindow::~BaseWindow() {}

void Quad::BaseWindow::Initialize(
    const std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)> &wndCallback)
{
    mWindowProc = wndCallback;
}

LRESULT CALLBACK Quad::BaseWindow::InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_NCCREATE:
    {
        CREATESTRUCT *createStrcut = (CREATESTRUCT *)lParam;
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)createStrcut->lpCreateParams);
        return true;
    }
    break;
    }

    Quad::BaseWindow *window = (Quad::BaseWindow *)(GetWindowLongPtrA(hwnd, GWLP_USERDATA));
    if (window)
        return window->GetWinProc()(hwnd, msg, wParam, lParam);
    else
        return DefWindowProc(hwnd, msg, wParam, lParam);
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
    if (AdjustWindowRect(&windowClientRect, windowStyle, false))
    {
        windowWidth = windowClientRect.right - windowClientRect.left;
        windowHeight = windowClientRect.bottom - windowClientRect.top;
    }

    HWND hwnd = CreateWindowW(wc.lpszClassName, windowName, windowStyle, 0, 0, windowWidth, windowHeight, 0, 0,
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

const std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)> &Quad::BaseWindow::GetWinProc()
{
    return mWindowProc;
    // TODO: 여기에 return 문을 삽입합니다.
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
