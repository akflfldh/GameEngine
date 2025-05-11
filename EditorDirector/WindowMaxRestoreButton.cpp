#include "WindowMaxRestoreButton.h"
#include"System.h"
#include"WindowResizeEvent.h"
#include"WindowRequestEvent.h"
#include"Controller.h"
#include"Window.h"
Quad::WindowMaxRestoreButton::WindowMaxRestoreButton(const std::string& name)
	:WindowButton(name)
{

}

void Quad::WindowMaxRestoreButton::Initialize()
{
	WindowButton::Initialize();
	
	
}

Quad::WindowMaxRestoreButton* Quad::WindowMaxRestoreButton::Create(Map* map, int mapLayerIndex)
{
	WindowMaxRestoreButton* ob = static_cast<WindowMaxRestoreButton*>(map->CreateObject("WindowMaxRestoreButton", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating();

	return ob;
}

void Quad::WindowMaxRestoreButton::OnEvent(Event* event)
{
	WindowButton::OnEvent(event);

	const std::string& eventName = event->GetName();

	if (eventName == "WindowResizeEvent")
	{
		HandleWindowResizeEvent(event);
	}


}

//void Quad::WindowMaxRestoreButton::SetWidthHeight(float width, float height)
//{
//	WindowButton::SetWidthHeight(width, height);
//
//}

void Quad::WindowMaxRestoreButton::ChangeWindowSize(UINT width, UINT height)
{
	WindowButton::ChangeWindowSize(width, height);
}

void Quad::WindowMaxRestoreButton::HandlePress()
{


	

}

void Quad::WindowMaxRestoreButton::HandleHover()
{
	SetColorItensity({ 1.5f,1.5f,1.5f });

}

void Quad::WindowMaxRestoreButton::HandleNotHover()
{
	SetColorItensity({ 1.0f,1.0f,1.0f });

}

void Quad::WindowMaxRestoreButton::HandlePressHover()
{
	SetColorItensity({ 2.0f,2.0f,2.0f });


}

void Quad::WindowMaxRestoreButton::HandlePressNotHover()
{
	SetColorItensity({ 1.5f,1.5f,1.5f });

}

void Quad::WindowMaxRestoreButton::HandleReleaseHover()
{
	//최소화하거나 할떄는 렌더링을멈춰야할텐데
	//다음함수를 바로 호출하는것이 아니라 ShowWindow()
	//뭔가 수행하기전에 처리해야할것이 있을수있으니 컨트롤러에게 요청해야한다.
	SetColorItensity({ 1.5f,1.5f,1.5f });
	//ReleaseCapture();
	WindowRequestEvent* windowRequestEvent = new WindowRequestEvent(EWindowRequestEventType::eWindowMaximize);
	GetSystem()->GetController()->AddEvent(windowRequestEvent);


}

void Quad::WindowMaxRestoreButton::HandleReleaseNotHover()
{
	SetColorItensity({ 1.0f,1.0f,1.0f });
	//ReleaseCapture();
}

void Quad::WindowMaxRestoreButton::InitCreating()
{
	WindowButton::InitCreating();
	SetDepth(99);
	SetKeepVisibleFlag(true);
	SetTexture("MaxRestoreButton.png");


	GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetEffectName("DefaultUi.effect");
	//SetWidthHeight(40, 40);
	SetSize(40, 40);

}

void Quad::WindowMaxRestoreButton::HandleWindowResizeEvent(Event* pEvent)
{

	WindowResizeEvent* windowResizeEvent = (WindowResizeEvent*)pEvent;
	ChangeWindowSize(windowResizeEvent->GetWindowWidth(), windowResizeEvent->GetWindowHeight());




}
