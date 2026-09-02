#pragma once

#include "Core/CoreDllExport.h"

#ifdef D3DX
#include <Windows.h>
#endif

namespace Core
{
class LogicalWindow;
}

namespace Core
{
class CORE_API_LIB GlobalAppHelper
{
  public:
    static GlobalAppHelper *GetInstance();

    void Initialize(HINSTANCE hinstance);

    static HINSTANCE GetHinstance();

    // 창에 mouse hover만 되어도 활성화
    //  input을 받아서 처리하는중인 LogicalWindow
    void SetCurrentMouseActiveLogicalWindow(Core::LogicalWindow *window);

    // 창을 클릭했을때 mouse down 시에  활성화
    void SetCurrentKeyboardActiveLogicalWindow(Core::LogicalWindow *window);

    void NotifyMouseCapture();
    void NotifyMouseReleaseCapture();

    void NotifyKeyboardCapture();
    void NotifyKeyboardReleaseCapture();

    Core::LogicalWindow *GetCurrMouseActiveLogicalWindow() const;

  private:
    static HINSTANCE mHinstance;

    Core::LogicalWindow *mCurrMouseActiveLogicalWindow;
    Core::LogicalWindow *mCurrKeyboardActiveLogicalWindow;
};

} // namespace Core
