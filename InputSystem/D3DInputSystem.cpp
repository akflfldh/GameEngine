#include "D3DInputSystem.h"

void Quad::D3DInputSystem::Update()
{

    POINT pt;
    GetCursorPos(&pt);

    mMouseContext.mScreenPosX = pt.x;
    mMouseContext.mScreenPosY = pt.y;

    ScreenToClient((HWND)mCurrWindowHandle, &pt);
    mMouseContext.mClientPosX = pt.x;
    mMouseContext.mClientPosY = pt.y;
}

void Quad::D3DInputSystem::OnMouseAction(UINT msg, WPARAM wParam, LPARAM lParam)
{

    switch (msg)
    {

    case WM_INPUT:
    {
    }
    break;
    }
}

void Quad::D3DInputSystem::EndFrame()
{

    InputSystem::EndFrame();
}

uint8_t Quad::D3DInputSystem::toScanKey(uint8_t vk) const
{
    return MapVirtualKeyA(vk, MAPVK_VK_TO_VSC);
}

uint8_t Quad::D3DInputSystem::toVirtualKey(uint8_t scanKey) const
{
    return MapVirtualKeyA(scanKey, MAPVK_VSC_TO_VK);
}
