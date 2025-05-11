#include "WindowCloseButton.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"WindowRequestEvent.h"
#include"System.h"
#include"Controller.h"
#include"WindowResizeEvent.h"
#include"Event/EventTable.h"
#include"Window.h"
#include"Mouse.h"

Quad::WindowCloseButton::WindowCloseButton(const std::string& name)
	:WindowButton(name)
{
}

void Quad::WindowCloseButton::Initialize()
{
	WindowButton::Initialize();
	


	

}

Quad::WindowCloseButton* Quad::WindowCloseButton::Create(Map* map, int mapLayerIndex)
{
	WindowCloseButton* ob = static_cast<WindowCloseButton*>(map->CreateObject("WindowCloseButton", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating();

	return ob;
}

void Quad::WindowCloseButton::OnEvent(Event* event)
{
	WindowButton::OnEvent(event);

	const std::string& name = event->GetName();
	if (name == "WindowResizeEvent")
	{
		HandleWindowResizeEvent(event);
	}


}

void Quad::WindowCloseButton::HandleHover()
{
	//색깔이 빨개지도록
	
	SetTexture("CloseButtonPressNotHover.png");
	//흰색반투명이 위에덮어지도록 등등
	


}

void Quad::WindowCloseButton::HandleNotHover()
{
	SetTexture("CloseButton.png");

}

void Quad::WindowCloseButton::HandlePressHover()
{
	SetTexture("CloseButtonPressHover.png");
//	GetModel().GetSubMesh()[0].mMateiral->SetDiffuseMap(TextureManager::GetTexture(L"CloseButtonPressHover.png"));

}

void Quad::WindowCloseButton::HandlePressNotHover()
{
	SetTexture("CloseButtonPressNotHover.png");
	//미리변수텍스처 포인터를 가리키면될듯.
	//GetModel().GetSubMesh()[0].mMateiral->SetDiffuseMap(TextureManager::GetTexture(L"CloseButtonPressNotHover.png"));

}

void Quad::WindowCloseButton::HandlePress()
{

	//SetCapture(GetWindow()->GetWindowHandle());
	//Mouse::CaptureObject(this);
	//색깔이 한번더 변하도록
	SetTexture("CloseButtonPressHover.png");
	//GetModel().GetSubMesh()[0].mMateiral->SetDiffuseMap(TextureManager::GetTexture(L"CloseButtonPressHover.png"));


}

void Quad::WindowCloseButton::HandleReleaseHover()
{
	
	//ReleaseCapture();
	//Mouse::ReleaseCaptureObject();
	//현재 윈도우멈춤,그리고 창닫음 요청전송, 
	WindowRequestEvent* windowRequestEvent = new WindowRequestEvent(EWindowRequestEventType::eWindowEnd);
	GetSystem()->GetController()->AddEvent(windowRequestEvent);




}

void Quad::WindowCloseButton::HandleReleaseNotHover()
{
	//ReleaseCapture();
	//Mouse::ReleaseCaptureObject();
	SetTexture("CloseButton.png");
	//GetModel().GetSubMesh()[0].mMateiral->SetDiffuseMap(TextureManager::GetTexture(L"CloseButton.png"));
}



//void Quad::WindowCloseButton::SetWidthHeight(float width, float height)
//{
//
//	WindowButton::SetWidthHeight(width, height);
//	UINT windowWidth = GetWindowWidth();
//	UINT windowHeight = GetWindowHeight();
//
//	//float positionXNdc = GetPositionXNdc();
//	//float positionYNdc = GetPositionYNdc();
//
//	//GetTransform().SetPositionLocal({ positionXNdc,positionYNdc,1.0f });
//
//
//
//	//GetTransform().
//
//	//부모자식관계를 구축하면 local  sacle,position의 값이 관계를 구축하는시점에서의 값을 기준으로해서 변경되는데.
//	//어떤 창은또 타이틀바가없을수있고
//	//타이틀바에있는 버튼은 그럼 사이즈수정을 하지않도록하면될거같은데
//	
//	//근데 위치는 타이틀바의 너비가 늘어나는데 그 오른쪽 끝에 계속 유지되어야하는데 .
//
//	//유지될거같은데.
//
//	//그리고 close button이 꼭 맨오른쪽 끝에 온다는 고정하는거는 적절하지않아. 
//	//창마다 다른위치에 존재하도록할수있으니.
//	//또 어떤창은 크게 가져갈수도있을거고
//	//어떤창에는 창,전체 모드 버튼이있는데 어떤창에는 없으니  최대최소 버튼의 위치가 옆으로 땡겨져야야하니깐 클래스내에서 고정시키는것은
//	//잘못된거지,.
//
//	//타이틀바같은 확정적인것은 상관없는데.
//
//	//자잘한 창들이 많이 존재할수있고 그러면 다양한 모습이 존재할텐데
//
//
//	//그니깐 위치랑, 크기는 윈도우 레이아웃 시스템에서 구축하도록해야하는거지
//	//이 윈도우는 타이틀바가있고 ,그 안에 왼쪾에 close button이 있다. 등등으로 말이지 .
//	// 애초에 엔티티 클래스내 확정할수있는게 아니야. 
//
//	//창(window)마다 windowLayout시스템 마다 적절하게 구축해야하겠지. 
//	//windowLayout을 상속해서 각각의 개별창들을 위한 레이아웃시스템을 만들고 그안에서 적절한 엔티티들을 위치 크기 ,변화를 구축한다.
//	//그러니깐 각 windowControllerEntity에 내부에서 어떤위치에 고정된다 크기가 고정된다 변화된다 라고 확정할수없는거야 .
//
//	//선택지를 줘서 결정하도록 할수는있겠다.
//	
//	//예를들면 크기가 윈도우 사이즈에상관없이 변하지않는다던가. 
//	//물론 확정적인것들도 있다 예를들면 타이틀바. 당연히 윈도우 사이즈가 변해도 변하지않으니깐. 
//
//
//	//플래그로  설정하게끔해서 그에따라 동작하도록 하면될거같다.
//	//사이즈가 윈도우 사이즈에따라 변화하는가, 아니면 윈도우 사이즈변화에도 고정되는가 ? 
//	//위치는 항상 고정되는가 ? 아니면 변화는가 ?
//
//	//또 엔티티의 위치는 크기자체는  어떻게되는가 그거는 윈도우마다 다른거지
//	//이런값들은 윈도우 layout system에서 초기에 필요한 엔티티들과 동시에 초기값을 설정해서 구축해야한다는거지.
//	
//	//예를 들면 게임플레이창은 타이틀바는 있는데 움직이는못하는데 그건타이틀바는아니지, 그럼 그냥 움직임 기능이없는 타이틀바가되겠네
//	//그리고 close button이있는지 없는거같고  ,중앙에 플레이버튼이있는데 유일하지 ..
//	//즉 창마다  다르다는거야
//
//
//
//
//
//
//	
//
//}


void Quad::WindowCloseButton::ChangeWindowSize(UINT width, UINT height)
{

	WindowButton::ChangeWindowSize(width, height);

	

}

void Quad::WindowCloseButton::InitCreating()
{
	WindowButton::InitCreating();


	//settexture할떄 textureManager를 직접가져오게하지말고, 이렇게 set diffuse map, 메서드 내부에서 
	//자체적으로 textureManager에서 가져오도록하는것도 괜찮을거같다.
	SetTexture("CloseButton.png");
	//GetModel().GetSubMesh()[0].mMateiral->SetDiffuseMap(TextureManager::GetTexture(L"CloseButton.png"));
	GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetEffectName("DefaultUi.effect");


	SetKeepVisibleFlag(true);
	SetKeepVisibleBaseVerticalLine(1);
	SetKeepVisibleBaseHorizontalLine(0);
	SetKeepVisibleBaseVerticalLineOffset(40);
	SetKeepVisibleBaseHorizontalLineOffset(40);

	//InitPositionNdc(0.0,0.5f);
	//사이즈
	// /
	SetSize(40, 40);
	//SetWidthHeight(40, 40);

	//위치,


	//타이틀바의 자식
}

void Quad::WindowCloseButton::HandleWindowResizeEvent(Event* pEvent)
{
	WindowResizeEvent* windowResizeEvent = (WindowResizeEvent*)pEvent;

	ChangeWindowSize(windowResizeEvent->GetWindowWidth(), windowResizeEvent->GetWindowHeight());







}

