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

    void Show(UINT showCmd = SW_SHOW);

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

    // 창의 UI 스레드에서 호출한다. 커서 표시 변경은 이 메서드로 통일하며,
    // 적용 대상 창의 선택과 비활성화 시 복원은 상위 컨트롤러가 담당한다.
    void SetCursorVisible(bool visible);

    // 생성된 창의 클라이언트 좌표 영역으로 제한한다. nullptr이면 제한을 해제한다.
    // UI 스레드에서 호출하며, 영역 변경 시 재적용과 비활성화 시 해제는 상위 컨트롤러가 담당한다.
    // 잘못된 영역이나 좌표 변환 실패 시 기존 제한을 변경하지 않고 false를 반환한다.
    bool ClipCursor(const RECT *clientRect);

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

    bool mInitMaxmized = false;
};
} // namespace Quad
