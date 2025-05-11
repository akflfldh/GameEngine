#include "TitleBarEntity.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"Controller.h"
#include"Event.h"
#include"MouseEvent.h"
#include"WindowRequestEvent.h"
#include"Event/EventTable.h"
#include"Window.h"
#include"System.h"
#include"WindowResizeEvent.h"
#include"Mouse.h"


Quad::TitleBarEntity::TitleBarEntity()
	:WindowControlEntity("titleBar")
{
}

Quad::TitleBarEntity::~TitleBarEntity()
{
}

void Quad::TitleBarEntity::Initialize()
{

	WindowControlEntity::Initialize();

	
	System* system = GetSystem();
	system->RegisterEntityToEvent("LButtonDown", this);
	system->RegisterEntityToEvent("LButtonUp", this);
	system->RegisterEntityToEvent("MouseMove", this);
	


}

void Quad::TitleBarEntity::HandleMouseMove(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;
	if (stateComponent->GetLButtonDownState())
	{
		WindowRequestEvent* windowRequestEvent = new WindowRequestEvent(EWindowRequestEventType::eWindowMove);
		Controller::AddEvent(GetControllerName(), windowRequestEvent);


	}









}

void Quad::TitleBarEntity::HandleLButtonUp(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetLButtonDownState())
	{
		stateComponent->SetLButtonDownState(false);
		WindowRequestEvent* windowRequestEvent = new WindowRequestEvent(EWindowRequestEventType::eWindowMoveEnd);
		Controller::AddEvent(GetControllerName(), windowRequestEvent);

	}

	


}

void Quad::TitleBarEntity::HandleWindowResize(Event* pEvent)
{


	WindowResizeEvent* windowResizeEvent = (WindowResizeEvent*)pEvent;
	ChangeWindowSize(windowResizeEvent->GetWindowWidth(), windowResizeEvent->GetWindowHeight());


}

void Quad::TitleBarEntity::HandleLButtonDown(Event* pEvent)
{
	MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);


	StateComponent* stateComponent = GetStateComponent();
	if (stateComponent->GetHoverState())
	{
		stateComponent->SetLButtonDownState(true);
	}
	else
	{
		stateComponent->SetLButtonDownState(false);
	}

	const std::vector<ObjectSmartPointer>& childObjectVector = GetChildObjectVector();
	for (auto & childObject : childObjectVector)
	{
		childObject->OnEvent(pEvent);
	}


}

void Quad::TitleBarEntity::HandleHover(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;

	if (mouseEvent->GetHoverState())
	{
		stateComponent->SetHoverState(true);
	}
	else
	{
		stateComponent->SetHoverState(false);
	}

}

//void Quad::TitleBarEntity::Initialize()
//{
//
//
//
//	//mWindowCloseButton.SetSystem(GetSystem());
//	//mWindowCloseButton.Initialize();
//
//
//
//	////부모 자식관계는 맞어, 그리고 위치는 영향을 받는게 맞지, 같이 움직이니깐, 근데 크기음,, 
//	////크기도 사실 타이틀바는 게속 변화하고있어 NDC공간에서의 스케일값이 계속변화고있거든 그럼 CLOSEBUTTON도 변해야돼 
//	////
//	
//
//
//
//}

Quad::TitleBarEntity* Quad::TitleBarEntity::Create(Map* map, int mapLayerIndex)
{
	TitleBarEntity* ob = static_cast<TitleBarEntity*>(map->CreateObject("TitleBarEntity", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating();

	return ob;
}

void Quad::TitleBarEntity::Update(float deltaTime)
{
	WindowControlEntity::Update(deltaTime);


	
}

void Quad::TitleBarEntity::OnEvent(Event* pEvent)
{

	WindowControlEntity::OnEvent(pEvent);

	const std::string & eventName = pEvent->GetName();

	/*if (eventName == "LButtonDown")
	{
		HandleLButtonDown(pEvent);
	}
	else if (eventName == "Hover")
	{
		HandleHover(pEvent);
	}
	
	else if (eventName == "LButtonUp")
	{
		HandleLButtonUp(pEvent);
	}*/

	if (eventName == "MouseMove")
	{
		HandleMouseMove(pEvent);

	}else
	if (eventName == "WindowResizeEvent")
	{
		HandleWindowResize(pEvent);
	}







}

void Quad::TitleBarEntity::SetSize(float width, float height)
{
	//titlebar는 width값은 무시
	//WindowControlEntity::SetWidthHeight(width, height); 
	
	UiEntity::SetSize(width, height);

	//GetTransform().SetScaleLocal({ width,height,1.0f });
	GetTransform().SetPositionWorld({ width / 2.0f, 3000 - height / 2.0f,100.0f });

}


void Quad::TitleBarEntity::ChangeWindowSize(UINT width, UINT height)
{

	WindowControlEntity::ChangeWindowSize(width, height);


}

void Quad::TitleBarEntity::InitCreating()
{

	WindowControlEntity::InitCreating();

	SetTexture("TitleBar.png");
	//Texture* texture = TextureManager::GetTexture(L"TitleBar.png");
	//GetModel().SetTextureToAllSubMesh(texture);
	GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetEffectName("DefaultUi.effect");

	SetKeepVisibleFlag(false);
	//SetKeepVisibleBaseHorizontalLine(0);
	//SetKeepVisibleBaseVerticalLine(0);
	//SetKeepVisibleBaseHorizontalLineOffset(20);//높이가 40이라 일단 가정
	//SetKeepVisibleBaseVerticalLineOffset(1500); //너비는 3000
	//600 x400을 기준으로 먼저초기화

	//SetWidthHeight(3000, 40.0f);
	SetSize(3000, 40);
	SetDepth(100.0f);


	//그후 시스템의 현재 윈도우 너비높이로변경
	System* system = GetSystem();

	ChangeWindowSize(system->GetClientWidth(), system->GetClientHeight());


	SetSelectKeepingFlag(false);

}
