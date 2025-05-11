#include "WindowMinButton.h"
#include"WindowResizeEvent.h"
#include"System.h"
#include"Window.h"
#include"Controller.h"
#include"WindowRequestEvent.h"



Quad::WindowMinButton::WindowMinButton(const std::string& name)
	:WindowButton(name)
{
}

void Quad::WindowMinButton::Initialize()
{
	WindowButton::Initialize();



}

Quad::WindowMinButton* Quad::WindowMinButton::Create(Map* map, int mapLayerIndex)
{
	WindowMinButton* ob = static_cast<WindowMinButton*>(map->CreateObject("WindowMinButton", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating();

	return ob;;
}

void Quad::WindowMinButton::OnEvent(Event* event)
{
	WindowButton::OnEvent(event);


	const std::string& eventName = event->GetName();
	if (eventName == "WindowResizeEvent")
	{
		HandleWindowResizeEvent(event);
	}
}

//void Quad::WindowMinButton::SetWidthHeight(float width, float height)
//{
//	WindowButton::SetWidthHeight(width, height);
//
//}

void Quad::WindowMinButton::ChangeWindowSize(UINT width, UINT height)
{
	WindowButton::ChangeWindowSize(width, height);
}

void Quad::WindowMinButton::HandlePress()
{
	//SetCapture(GetWindow()->GetWindowHandle());


}

void Quad::WindowMinButton::HandleHover()
{
	SetColorItensity({ 1.5f,1.5f,1.5f });


}

void Quad::WindowMinButton::HandleNotHover()
{

	SetColorItensity({ 1.0f,1.0f,1.0f });
}

void Quad::WindowMinButton::HandlePressHover()
{

	SetColorItensity({ 2.0f,2.0f,2.0f });
}

void Quad::WindowMinButton::HandlePressNotHover()
{

	SetColorItensity({ 1.5f,1.5f,1.5f });

}

void Quad::WindowMinButton::HandleReleaseHover()
{


	SetColorItensity({ 1.5f,1.5f,1.5f });

	//현재 윈도우멈춤,그리고 창닫음 요청전송, 
	WindowRequestEvent* windowRequestEvent = new WindowRequestEvent(EWindowRequestEventType::eWindowMinimize);
	GetSystem()->GetController()->AddEvent(windowRequestEvent);



}

void Quad::WindowMinButton::HandleReleaseNotHover()
{
//	ReleaseCapture();

	SetColorItensity({ 1.0f,1.0f,1.0f });
}

void Quad::WindowMinButton::InitCreating()
{
	WindowButton::InitCreating();

	SetTexture("MinButton.png");

	GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetEffectName("DefaultUi.effect");
	SetKeepVisibleFlag(true);
	SetDepth(99);
	//SetWidthHeight(40, 40);
	SetSize(40, 40);



}

void Quad::WindowMinButton::HandleWindowResizeEvent(Event* pEvent)
{
	WindowResizeEvent* windowResizeEvent = (WindowResizeEvent*)pEvent;

	ChangeWindowSize(windowResizeEvent->GetWindowWidth(), windowResizeEvent->GetWindowHeight());


}
