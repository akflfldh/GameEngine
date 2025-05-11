#include "ContentIProjecttemUiEntity.h"
#include"Map/Map.h"
#include"MouseEvent.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"ProjectDirector.h"

void Quad::ContentIProjecttemUiEntity::Initialize()
{
	ContentItemBaseUiEntity::Initialize();

}

Quad::ContentIProjecttemUiEntity* Quad::ContentIProjecttemUiEntity::Create(Map* map, int mapLayerIndex, ContentItem* item)
{
	ContentIProjecttemUiEntity* ob = static_cast<ContentIProjecttemUiEntity*>(map->CreateObject("ContentIProjecttemUiEntity", mapLayerIndex));
	ob->Initialize();
	ob->InitCreating(item);

	return ob;
}

void Quad::ContentIProjecttemUiEntity::Update(float deltaTime)
{
	ContentItemBaseUiEntity::Update(deltaTime);

}

void Quad::ContentIProjecttemUiEntity::OnEvent(Event* event)
{
	ContentItemBaseUiEntity::OnEvent(event);

	const std::string& eventName = event->GetName();
	if (eventName == "LButtonDownDouble")
	{
		HandleLButtonDownDouble(event);
	}




}

void Quad::ContentIProjecttemUiEntity::InitCreating(ContentItem* item)
{
	ContentItemBaseUiEntity::InitCreating(item, EType::eFileUiVersion);
}

void Quad::ContentIProjecttemUiEntity::HandleLButtonDown(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);



	bool hoverState =stateComponent->GetHoverState();
	if (hoverState)
	{
		stateComponent->SetLButtonDownState(true);
		stateComponent->SetSelectState(true);
		SetTexture(TextureManager::GetTexture("Blue.png"));
		
	}
	else
	{
		//stateComponent->SetLButtonDownState(false);
		stateComponent->SetSelectState(false);
		SetTexture(TextureManager::GetTexture("Black.png"));

	}

}

void Quad::ContentIProjecttemUiEntity::HandleLButtonUp(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();

	stateComponent->SetLButtonDownState(false);

//	bool hoverState = stateComponent->GetHoverState();



}

void Quad::ContentIProjecttemUiEntity::HandleHover(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);
	
	bool hoverState = mouseEvent->GetHoverState();
	stateComponent->SetHoverState(mouseEvent->GetHoverState());
	
	if (!stateComponent->GetSelectState())
	{
		if (hoverState)
		{
			//SetColorItensity({ 2.0F,2.0F,2.0F });
			SetTexture(TextureManager::GetTexture("DarkGray.png"));
		}
		else
		{
			SetTexture(TextureManager::GetTexture("Black.png"));
		}
	}


}

void Quad::ContentIProjecttemUiEntity::HandleLButtonDownDouble(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();


	bool hoverState = stateComponent->GetHoverState();
	if (hoverState)
	{
		SetTexture(TextureManager::GetTexture("Red.png"));

		//이Project를 실행하겠다(Load 한다)
		ProjectDirector::SelectProject(static_cast<ProjectMetaData*>(GetContentItem()));
	}



}


