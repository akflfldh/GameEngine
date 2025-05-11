#include "WindowButton.h"
#include"Event.h"
#include"ResourceManager/TextureManager/TextureManager.h"

#include"MouseEvent.h"
#include"Mouse.h"
#include"Map/Map.h"
Quad::WindowButton::WindowButton(const std::string& name)
	:WindowControlEntity(name)
{
	
}

void Quad::WindowButton::Initialize()
{
	WindowControlEntity::Initialize();
	
	//GetModel().GetSubMesh()[0].mMateiral->SetDiffuseMap(TextureManager::GetTexture())

	RegisterAcceptEvent("LButtonDown");
	RegisterAcceptEvent("LButtonUp");
	RegisterAcceptEvent("MouseMove");




}

Quad::WindowButton* Quad::WindowButton::Create(Map* map, int mapLayerIndex)
{
	WindowButton* ob = static_cast<WindowButton*>(map->CreateObject("WindowButton", mapLayerIndex));

//	ob->Initialize();
	ob->InitCreating();

	return ob;
}

void Quad::WindowButton::Update(float deltaTime)
{
	WindowControlEntity::Update(deltaTime);
	
	






}

void Quad::WindowButton::OnEvent(Event* pEvent)
{
	WindowControlEntity::OnEvent(pEvent);

	const std::string& eventName = pEvent->GetName();

	if (eventName == "MouseMove")
	{
		HandleMouseMove(pEvent);
	}


}

//void Quad::WindowButton::SetWidthHeight(float width, float height)
//{
//
//	WindowControlEntity::SetWidthHeight(width, height);
//
//}

void Quad::WindowButton::ChangeWindowSize(UINT width, UINT height)
{
	WindowControlEntity::ChangeWindowSize(width, height);


	



}

//void Quad::WindowButton::SetSelectState(bool state)
//{
//	mSelectState = state;
//}
//
//bool Quad::WindowButton::GetSelectState() const
//{
//	return mSelectState;
//}

void Quad::WindowButton::InitCreating()
{
	WindowControlEntity::InitCreating();


}

void Quad::WindowButton::HandleMouseMove(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseMoveEvnet = (MouseEvent*)pEvent;
	
	if (stateComponent->GetLButtonDownState())
	{

		if (stateComponent->GetHoverState())
		{
			HandlePressHover();
		}
		else
		{
			HandlePressNotHover();
		}
	}
	else
	{
		
	}
	


}

void Quad::WindowButton::HandleLButtonUp(Event* pEvent)
{
	
	MouseEvent* lButtonUpEvnet = (MouseEvent*)pEvent;

	StateComponent* stateComponent = GetStateComponent();
	if (stateComponent->GetLButtonDownState())
	{

		stateComponent->SetLButtonDownState(false);
		if (stateComponent->GetHoverState())
		{
			HandleReleaseHover();
		}
		else
		{
			HandleReleaseNotHover();
		}

		Mouse::ReleaseCaptureObject(this);
	}

}

void Quad::WindowButton::HandleHover(Event* pEvent)
{
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;
	StateComponent* stateComponent = GetStateComponent();
	if (mouseEvent->GetHoverState())
	{
		stateComponent->SetHoverState(true);
		if(!stateComponent->GetLButtonDownState())
			HandleHover();
	}
	else
	{
		stateComponent->SetHoverState(false);
		if (!stateComponent->GetLButtonDownState())
		{
			HandleNotHover();
		}
	}




}

void Quad::WindowButton::HandleLButtonDown(Event* pEvent)
{
	StateComponent* stateComponenet = GetStateComponent();

	if (stateComponenet->GetHoverState())
	{
		Mouse::CaptureObject(this);
		stateComponenet->SetLButtonDownState(true);
		HandlePress();
	}
}
