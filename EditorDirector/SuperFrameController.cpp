#include "SuperFrameController.h"

#include<Window/BaseWindow.h>

#include<Core/GlobalAppHelper.h>


#include<Core/InputSystem.h>
#include<UISystem.h>


Quad::SuperFrameController::~SuperFrameController()
{


}


Quad::SuperFrameController* Quad::SuperFrameController::GetInstance()
{
	static SuperFrameController controller;

	return &controller;

}


void Quad::SuperFrameController::Initialize()
{

	mWindow = new BaseWindow(GlobalAppHelper::GetHinstance());

    mWindow->Initialize(std::bind(&SuperFrameController::WndProc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	mWindow->CreateWindowClass(L"FrameWindow",L"FrameWindow");


    InputSystem* inputSystem = InputSystem::GetInstance();
    inputSystem->RegisterMouseAction(Quad::EMouseInput::eLButtonDown, "Test", this, &SuperFrameController::TestLButtonDownCallback,1);

  //  mUISystem = UI::UISystem::GetInstance();

}

void Quad::SuperFrameController::PreUpdate()
{
}

void Quad::SuperFrameController::Update(float deltaTime)
{




}

void Quad::SuperFrameController::EndUpdate()
{
}

void Quad::SuperFrameController::Draw()
{





}



Quad::BaseWindow* Quad::SuperFrameController::GetWindow()
{
	return mWindow;
}

void Quad::SuperFrameController::TestLButtonDownCallback()
{

    MessageBox(mWindow->GetWindowHandle(), L"마우스 클릭!", L"알림", MB_OK);

}


LRESULT CALLBACK Quad::SuperFrameController::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    InputSystem * inputSystem =  InputSystem::GetInstance();

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
    case WM_LBUTTONDOWN:
        inputSystem->OnMouseEvent(Quad::EMouseInput::eLButtonDown,1);

        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}