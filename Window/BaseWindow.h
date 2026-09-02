#pragma once

#include <Windows.h>

#include <functional>
class IWindowEventHandler;

namespace Quad
{

class BaseWindow
{
  public:
    BaseWindow(HINSTANCE hInstance);
    virtual ~BaseWindow();
    // 다른 창들은 항상 playmode true인것 //항상작동하니
    // gamePlayWindow는 에디터모드가있고 ,게임플레이모드 두가지모드사이를 전환할수있다.

    void Initialize();

    bool CreateWindowClass(LPCWSTR windowClassName, LPCWSTR windowName, DWORD windowStyle = WS_OVERLAPPEDWINDOW,
                           UINT windowClassStyle = CS_HREDRAW | CS_VREDRAW);

    void SetVisible(bool flag);

    void SetIWindowEventHandler(IWindowEventHandler *windowEventHandler);

  public:
    HINSTANCE GetHInstance() const;
    HWND GetWindowHandle() const;
    void SetWindowHandle(HWND handle);

    void SetClientWidth(unsigned short width);
    void SetClientHeight(unsigned short height);

    unsigned short GetClientWidth() const;
    unsigned short GetClientHeight() const;

    void SetMaxClientWidth(unsigned short width);
    void SetMaxClientHeight(unsigned short height);

    unsigned short GetMaxClientWidth() const;
    unsigned short GetMaxClientHeight() const;

    void SetMinClientWidth(unsigned short width);
    void SetMinClientHeight(unsigned short height);

    unsigned short GetMinClientWidth() const;
    unsigned short GetMinClientHeight() const;

    void SetMouseCapture(bool flag);
    void SetKeyboardCapture(bool flag);

    //
    void ShutDown();

  protected:
    static LRESULT CALLBACK InnerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    bool RegisterRawInputDevice();

    int mClientMousePosX = 0;
    int mClientMousePosY = 0;

  private:
    HINSTANCE mHInstance;
    HWND mWindowHandle;
    DWORD mWinStyle;

    unsigned short mClientWidth;
    unsigned short mClientHeight;

    unsigned short mMaxClientWidth;
    unsigned short mMaxClientHeight;

    unsigned short mMinClientWidth;
    unsigned short mMinClientHeight;

    float mWindowPositionX;
    float mWindowPositionY;

    IWindowEventHandler *mIWindowEventHandler;

    POINT mLastClientPos = {0, 0};

    bool mInitialized = false;
};
} // namespace Quad
