#include "SuperAssetBrowerController.h"

#include<Window/BaseWindow.h>

#include<Core/GlobalAppHelper.h>
#include<functional>

#include"Core/InputSystem.h"
 

Quad::SuperAssetBrowerController* Quad::SuperAssetBrowerController::GetInstance()
{

    static SuperAssetBrowerController controller;
    return &controller;

}

Quad::SuperAssetBrowerController::SuperAssetBrowerController()
{


}

Quad::SuperAssetBrowerController::~SuperAssetBrowerController()
{

}

void Quad::SuperAssetBrowerController::Initialize()
{
	mWindow = new BaseWindow(GlobalAppHelper::GetHinstance());

	mWindow->Initialize(std::bind(&SuperAssetBrowerController::WndProc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	mWindow->CreateWindowClass(L"AssetBrower",L"AssetBrower");

    auto inputSystem = InputSystem::GetInstance();
    inputSystem->RegisterMouseAction(EMouseInput::eRButtonDown, "Asset", this, &SuperAssetBrowerController::TestRButtonDown, 0);

}

Quad::BaseWindow* Quad::SuperAssetBrowerController::GetWindow()
{
	return mWindow;
}

void Quad::SuperAssetBrowerController::TestRButtonDown()
{
    MessageBox(mWindow->GetWindowHandle(), L"마우스 R 클릭!", L"알림", MB_OK);
}


LRESULT CALLBACK Quad::SuperAssetBrowerController::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    InputSystem* inputSystem = InputSystem::GetInstance();

    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        TextOut(hdc, 10, 10, L"Hello, Windows!", 16);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

        // 기타 메시지들 추가 가능
    case WM_RBUTTONDOWN:
        inputSystem->OnMouseEvent(Quad::EMouseInput::eRButtonDown, 0);

        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }



}