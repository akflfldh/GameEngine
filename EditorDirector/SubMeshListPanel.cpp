#include "SubMeshListPanel.h"
#include"GameObjectSelectEvent.h"
#include"EventDispatcher.h"
#include"TextBoxPanel.h"

#include"MouseEvent.h"
Quad::SubMeshListPanel::SubMeshListPanel(const std::string& name)
	:PanelUiEntity(name), mListNum(0), mCurrentSubMeshindex(0),mTextBoxPanel(nullptr),mSelectedObject(nullptr)
{

}

Quad::SubMeshListPanel::~SubMeshListPanel()
{
}

void Quad::SubMeshListPanel::Initialize()
{
	PanelUiEntity::Initialize();
	
}


Quad::SubMeshListPanel* Quad::SubMeshListPanel::Create(Map* map, int mapLayerIndex, float width, float height)
{
	SubMeshListPanel* ob = static_cast<SubMeshListPanel*>(map->CreateObject("SubMeshListPanel", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating(width, height);


	return ob;
}

void Quad::SubMeshListPanel::Update(float deltaTime)
{



}

void Quad::SubMeshListPanel::OnEvent(Event* event)
{
	const std::string& eventName = event->GetName();

	if (eventName == "GameObjectSelect")
	{
		HandleGameObjectSelect(event);
	}
	else
	{

		PanelUiEntity::OnEvent(event);
	}
}

void Quad::SubMeshListPanel::SetListNum(int i)
{
	mListNum = i;
	mTextBoxPanel->SetText("SubMesh" + std::to_string(i));

}

int Quad::SubMeshListPanel::GetListNum() const
{
	return 0;
}

void Quad::SubMeshListPanel::SetDrawFlag(bool flag)
{

	Object::SetDrawFlag(flag);
	mTextBoxPanel->SetDrawFlag(flag);
}

void Quad::SubMeshListPanel::SetActiveFlag(bool flag)
{
	Object::SetActiveFlag(flag);
	mTextBoxPanel->SetActiveFlag(flag);
}

void Quad::SubMeshListPanel::SetStencilRefValue(unsigned char value)
{
	Object::SetStencilRefValue(value);
	mTextBoxPanel->SetStencilRefValue(value);

}

void Quad::SubMeshListPanel::InitCreating(float width, float height)
{
	PanelUiEntity::InitCreating();


	SetSize(width, height);

	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	//	mTextBoxPanel = new TextBoxPanel(GetName() + "_TextBoxPanel");
	//mTextBoxPanel = static_cast<TextBoxPanel*>(map->CreateObject("TextBoxPanel"));
	mTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, width, height);

	mTextBoxPanel->GetTransform().SetIndependentScaleFlag(false);
	mTextBoxPanel->GetTransform().SetIndependentRotationFlag(true);
	mTextBoxPanel->SetTexture("Blue.png");
	//mTextBoxPanel->SetDrawFlag(true);
	mTextBoxPanel->SetSelectAvailableFlag(false);

	AddChildPanelUiEntity(mTextBoxPanel);
	mTextBoxPanel->SetPosition(0, 0, -1);
	//SetDrawFlag(false);
}


void Quad::SubMeshListPanel::HandleLButtonDown(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetHoverState())
	{

		if (mCurrentSubMeshindex != mListNum)
		{

			GameObjectSelectEvent* selectEvent = new GameObjectSelectEvent;
			selectEvent->SetSelectedObject(mSelectedObject);
			selectEvent->SetCurrentSubMeshIndex(mListNum);
			EventDispatcher::SendEvent(selectEvent, ESystemID::eAttributeMainSystem);

		}


		auto callback = GetEventCallback("LButtonDown");
		if (callback != nullptr)
			(*callback)(pEvent);


	}
}


void Quad::SubMeshListPanel::HandleGameObjectSelect(Event* pEvent)
{

	GameObjectSelectEvent* selectEvent = (GameObjectSelectEvent*)pEvent;

	Object* selectedObject = selectEvent->GetSelectObject();

	if (selectedObject->GetObjectType ()== EObjectType::eEntity)
	{
		mSelectedObject = selectedObject;
		mCurrentSubMeshindex = selectEvent->GetCurrentSubMeshIndex();
	}


}

void Quad::SubMeshListPanel::HandleHover(Event* pEvent)
{

	MouseEvent* mouseEvent = (MouseEvent*)pEvent;

	PanelUiEntity::HandleHover(pEvent);


	if (mouseEvent->GetHoverState())
	{
		mTextBoxPanel->SetColorItensity({ 2.0f,2.0f,2.0f });
	}
	else
	{
		mTextBoxPanel->SetColorItensity({ 1.0f,1.0f,1.0f });
	}


}
