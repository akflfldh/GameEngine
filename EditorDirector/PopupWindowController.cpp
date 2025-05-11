#include "PopupWindowController.h"
#include"System/PopupWindowUiSystem.h"
#include"RenderSystem.h"
#include"PopupWindow.h"
#include"System/PopupWindowUiSystem.h"

#include"PopupWindowDefulatMsgHandlerComponent.h"
#include"MouseHandlerComponent.h"
#include"KeyInputHandlerComponent.h"
#include"KeyBoard.h"

Quad::PopupWindowController::PopupWindowController(RenderSystem* renderSystem)
	:BaseWindowController(renderSystem),mKeepVisiblity(false), mVisibleState(false)
{
}

void Quad::PopupWindowController::Initialize(PopupWindow* window, PopupWindowUiSystem* uiSystem)
{
	BaseWindowController::Initialize();

	mPopupWindow = window;
	//mRenderSystem = renderSystem;
	mUiSystem = uiSystem;

	mMonitorHandle = MonitorFromWindow(mPopupWindow->GetWindowHandle(), MONITOR_DEFAULTTOPRIMARY);

	mUiSystem->SetActiveState(true);

	AddMsgHanlderComponent(new PopupWindowDefulatMsgHandlerComponent(this));
	AddMsgHanlderComponent(new MouseHandlerComponent(this));
	AddMsgHanlderComponent(new KeyInputHandlerComponent(this));

}



void Quad::PopupWindowController::Update(float deltaTime)
{
	Event* pEvent = nullptr;

	while (1)
	{
		Event* pEvent = GetEvent();//or unique_ptr 로 리턴 그럼마지막에 delete를 명시하는것을 체크하지않아도된다.
		if (pEvent == nullptr)
			break;

		HandleEvent(pEvent);
		delete pEvent;
	}



	mUiSystem->Update(deltaTime);

	


}

void Quad::PopupWindowController::UploadObjectToRenderSystem()
{
	auto renderSystem =	GetRenderSystem();

	if (!renderSystem->GetRenderState())
		return;

	renderSystem->PreUpdate();

	//이렇게 렌터 항목,아이템들을 넘기는것을 함수로처리하자.
	//std::vector<Object*> objectVector;

	//mUiSystem->GetEntity(objectVector);
	const std::vector<MapLayer>  objectVector =  mUiSystem->GetEntity();
	//mRenderSystem->SetEntityVector(objectVector, ESystemType::eUiSystem,0);
	renderSystem->SetMapLayerVector(objectVector, ESystemType::eMainSystem);
	//objectVector.clear();


	RenderSettingItem renderSettingItem;

	mUiSystem->GetRenderSettingItem(renderSettingItem);
	renderSystem->SetRenderSettingItem(renderSettingItem, ESystemType::eMainSystem);

	renderSystem->Update();

}

void Quad::PopupWindowController::EndUpdate(float deltaTime)
{
	mUiSystem->EndUpdate(deltaTime);
}

void Quad::PopupWindowController::Draw()
{
	auto renderSystem = GetRenderSystem();
	renderSystem->Draw();
}

void Quad::PopupWindowController::OnResize(int clientWidth, int clientHeight)
{
	
	auto renderSystem = GetRenderSystem();
	mUiSystem->OnResize(clientWidth, clientHeight);
	renderSystem->OnResize(clientWidth, clientHeight);

}

void Quad::PopupWindowController::HandleEvent(Event* pEvent)
{

	const std::string& eventName = pEvent->GetName();
	
	if (eventName == "KeyDown")
	{
		Object * object = KeyBoard::GetCapturedObject();
		if (object != nullptr)
		{
			object->OnEvent(pEvent);
			return;
		}

	}
	
	if (eventName == "KeyUp")
	{
		int a = 2;
	}

	mUiSystem->HandleEvent(pEvent);


}

void Quad::PopupWindowController::ActivateWindow(int popupPanelID, int screenPosX, int screenPosY)
{
	auto instance = GetInstance();

	instance->mUiSystem->ActivatePanel(popupPanelID);
	std::pair<int,int> size=  instance->mUiSystem->GetPopupPanelSize(popupPanelID);
	


	

	instance->AdjustWindowPos(screenPosX, screenPosY ,size.first,size.second);


	
	
}

void Quad::PopupWindowController::DeActivateWindow()
{
	auto instance = GetInstance();
	instance->mPopupWindow->SetVisibilityState(false);

	
}

Quad::PopupWindow* Quad::PopupWindowController::GetWindow() const
{
	return mPopupWindow;
}

int Quad::PopupWindowController::RegisterWindowInActiveCallback(const std::function<void()>& callback)
{
	auto instance = GetInstance();
	instance->mPopupInActiveCallbackTable.push_back({ true,callback });

	return instance->mPopupInActiveCallbackTable.size() - 1;

}

void Quad::PopupWindowController::SetStateWindowInActiveCallback(int index,bool state)
{
	auto instance = GetInstance();
	instance->mPopupInActiveCallbackTable[index].first = state;

}

void Quad::PopupWindowController::CallInActiveCallback()
{
	for (auto& callback : mPopupInActiveCallbackTable)
	{
		if (callback.first)
			callback.second();
	}
}

void Quad::PopupWindowController::HandleInActiveMsg()
{

	if (GetKeepVisiblity())
	{


	}
	else
	{
		SetWindowVisibilityState(false);
	
	}

	CallInActiveCallback();

}


void Quad::PopupWindowController::SetWindowVisibilityState(bool flag)
{

	HWND hwnd = mPopupWindow->GetWindowHandle();
	SetVisiblity(flag);
	if (flag)
	{
		ShowWindow(hwnd, SW_SHOW);
	}
	else
	{
		ShowWindow(hwnd, SW_HIDE);
	}

	UpdateWindow(hwnd);

}

void Quad::PopupWindowController::SetKeepVisiblity(bool state)
{
	mKeepVisiblity = state;

}

bool Quad::PopupWindowController::GetKeepVisiblity() const
{
	return mKeepVisiblity;
}

void Quad::PopupWindowController::SetVisiblity(bool state)
{
	mVisibleState = state;

}

bool Quad::PopupWindowController::GetVisiblity() const
{
	return mVisibleState;
}

void Quad::PopupWindowController::AdjustWindowPos(int screenPosX, int screenPosY ,int width ,int height)
{
	HWND hwnd = mPopupWindow->GetWindowHandle();


	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfo(mMonitorHandle, &monitorInfo);


	RECT popupWindowRect;
	popupWindowRect.left = screenPosX;
	popupWindowRect.right = popupWindowRect.left + width;
	popupWindowRect.top = screenPosY;
	popupWindowRect.bottom = popupWindowRect.top + height;



	if (monitorInfo.rcWork.right < popupWindowRect.right)
	{
		//차이만큼 왼쪽으로이동한다.
		screenPosX -= (popupWindowRect.right - monitorInfo.rcWork.right);

	}

	if (monitorInfo.rcWork.bottom < popupWindowRect.bottom)
	{
		//차이만큼 위로이동한다.
		screenPosY -= (popupWindowRect.bottom - monitorInfo.rcWork.bottom);
	}
	SetWindowPos(hwnd, HWND_TOPMOST, screenPosX, screenPosY, width, height, 0);
	mPopupWindow->SetVisibilityState(true);
	

}

void Quad::PopupWindowController::Initialize()
{
	Controller::Initialize();

}
