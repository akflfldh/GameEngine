#include "ListPanel.h"
#include"Map/Map.h"

#include"Event.h"
#include"MouseEvent.h"



Quad::ListPanel::ListPanel()
	:PanelUiEntity(), mChildListItemMaxNum(0), mChildListCurrentNum(0), mChildPanelUiEntityHeight(0.0f)
{
}

Quad::ListPanel::~ListPanel()
{
}

void Quad::ListPanel::Initialize()
{
	PanelUiEntity::Initialize();

}

Quad::ListPanel* Quad::ListPanel::Create(Map* map, int mapLayerIndex, float width, float height, int childPanelUiEntityHeight)
{

	ListPanel* ob = static_cast<ListPanel*>(map->CreateObject("ListPanel", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating(width,height, childPanelUiEntityHeight);

	return ob;

}

void Quad::ListPanel::OnEvent(Event* pEvent)
{

	const std::string eventName = pEvent->GetName();
	if (eventName == "MouseMove")
	{
		int a = 2;
	}

	PanelUiEntity::OnEvent(pEvent);
	








}

bool Quad::ListPanel::AddChildPanelEntity(PanelUiEntity* childPanelUiEntity)
{/*
	if (mChildListCurrentNum >= mChildListItemMaxNum)
		return false;



	float panelWidth = GetWidth();
	float panelHeight = GetHeight();
	
	childPanelUiEntity->SetSize(panelWidth, mChildPanelUiEntityHeight);
	Transform & childPanelUiEntityTransform = childPanelUiEntity->GetTransform();
	childPanelUiEntityTransform.SetIndependentScaleFlag(true);
	childPanelUiEntityTransform.SetIndependentRotationFlag(true);


	AddChildPanelUiEntity(childPanelUiEntity);
	mChildListCurrentNum++;

	childPanelUiEntity->SetPosition(0, CalcChildPanelUiEntityPosLocalY(mChildListCurrentNum-1), -1);*/

	return true;


}

void Quad::ListPanel::InitCreating(float width, float height, int childPanelUiEntityHeight)
{

	PanelUiEntity::InitCreating();
	

	SetWidthHeightLocal(width, height);
	//SetSize(width, height);
	mChildPanelUiEntityHeight = childPanelUiEntityHeight;


	mChildListItemMaxNum = height / childPanelUiEntityHeight;
}

float Quad::ListPanel::CalcChildPanelUiEntityPosLocalY(int childPanelUiEntityIndex)
{
	//float scrollListPanelHeight = 0; GetHeight();

	//float childPanelUiEntityPosLocalY = scrollListPanelHeight / 2 - mChildPanelUiEntityHeight / 2 - mChildPanelUiEntityHeight * childPanelUiEntityIndex;


	//return childPanelUiEntityPosLocalY;

	return 0;
}

void Quad::ListPanel::HandleHover(const MouseInputData& moustInputData)
{
	//MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);
	//bool hoverState =	mouseEvent->GetHoverState();
	//if (hoverState == false)
	//{
	//	int a = 2;
	//}

	//ListPanel* ob = this;
	//PanelUiEntity::HandleHover(pEvent);
	///*MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);

	//StateComponent* stateComponent = GetStateComponent();


	//bool hoverState = mouseEvent->GetHoverState();
	//stateComponent->SetHoverState(hoverState);

	//auto callback = GetEventCallback("Hover");
	//if (callback != nullptr)
	//{
	//	(*callback)(pEvent);
	//}

	//if (mouseEvent->GetHoverState() == false)
	//{
	//	const std::vector<ObjectSmartPointer>& childObjectSmartPointerVector = GetChildObjectVector();
	//	for (auto& childPanelEntity : childObjectSmartPointerVector)
	//	{
	//		childPanelEntity->OnEvent(pEvent);
	//	}
	//}*/
	//



}

void Quad::ListPanel::HandleMouseMove(const MouseInputData& moustInputData)
{
	PanelUiEntity::HandleMouseMove(moustInputData);



}
