#include "ClassListItemPanelEntity.h"
#include"Map/Map.h"
#include"TextBoxPanel.h"
#include"Event.h"
#include"MouseEvent.h"

Quad::ClassListItemPanelEntity::ClassListItemPanelEntity()
	:PanelUiEntity("")
{

}

Quad::ClassListItemPanelEntity::~ClassListItemPanelEntity()
{
}

void Quad::ClassListItemPanelEntity::Initialize()
{
	PanelUiEntity::Initialize();


}

Quad::ClassListItemPanelEntity* Quad::ClassListItemPanelEntity::Create(Map* map, int mapLayerIndex,const std::string & className,float width, float height)
{
	ClassListItemPanelEntity* ob = static_cast<ClassListItemPanelEntity*>(map->CreateObject("ClassListItemPanelEntity", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating(map,mapLayerIndex,className, width,height);

	return ob;
}

void Quad::ClassListItemPanelEntity::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);

}

void Quad::ClassListItemPanelEntity::OnEvent(Event* event)
{
	const std::string& eventName = event->GetName();


	if (eventName == "Hover")
	{
		HandleHover(event);
	}
	else if (eventName == "LButtonDown")
	{
		HandleLButtonDown(event);
	}






}

void Quad::ClassListItemPanelEntity::SetStencilRefValue(unsigned char value)
{
	PanelUiEntity::SetStencilRefValue(value);

	mTextBoxPanel->SetStencilRefValue(value);
}

std::string Quad::ClassListItemPanelEntity::GetEntityClassName() const
{
	return mTextBoxPanel->GetText();
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::ClassListItemPanelEntity::InitCreating(Map * map, int mapLayerIndex, const std::string& className, float width, float height)
{
	PanelUiEntity::InitCreating();
	SetSize(width, height);
	GetTransform().SetIndependentRotationFlag(true);
	GetTransform().SetIndependentScaleFlag(true);
	SetSelectAvailableFlag(false);
	SetTexture("DarkGray.png");

	mTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, width, height);
	mTextBoxPanel->GetTransform().SetIndependentScaleFlag(true);
	mTextBoxPanel->GetTransform().SetIndependentRotationFlag(true);
	mTextBoxPanel->SetSelectAvailableFlag(false);
	mTextBoxPanel->SetPosition(0, 0, -1);
	mTextBoxPanel->SetText(className);
	mTextBoxPanel->SetTexture("Transparent.png");
	//mTextBoxPanel->SetTextColor({ 0.0f,0.0f,0.0f });
	this->AddChildObject(mTextBoxPanel);



}

void Quad::ClassListItemPanelEntity::HandleHover(Event* event)
{

	StateComponent* stateComponent = GetStateComponent();

	MouseEvent* mouseEvent = static_cast<MouseEvent*>(event);
	
	
	bool hoverState = mouseEvent->GetHoverState();
	stateComponent->SetHoverState(hoverState);

	//if (hoverState)
	//{
	//	SetColorItensity({ 2.0F,2.0F,2.0f });
	//}
	//else
	//{
	//	SetColorItensity({ 1.0F,1.0F,1.0f });
	//}


}

void Quad::ClassListItemPanelEntity::HandleLButtonDown(Event* event)
{
	StateComponent* stateComponent = GetStateComponent();

	MouseEvent* mouseEvent = static_cast<MouseEvent*> (event);



	if (stateComponent->GetHoverState())
	{
		SetColorItensity({ 2.0f,2.0f,2.0f });
	}
	else
	{

		SetColorItensity({ 1.0F,1.0F,1.0f });
	}


}






