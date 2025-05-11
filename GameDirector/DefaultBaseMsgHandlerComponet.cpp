#include "DefaultBaseMsgHandlerComponet.h"

LRESULT DefaultBaseMsgHandlerComponet::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& ret)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
