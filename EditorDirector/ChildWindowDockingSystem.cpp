#include "System/ChildWindowDockingSystem.h"
#include"EditorDirector/DockingPanel.h"
#include"EditorDirector/DockingSelectBox.h"
#include"EditorDirector/FrameWindowController.h"
#include"Component/UiColliderComponent.h"
#include"ObjectFactory/BaseObjectFactory.h"
#include"EditorDirector/Window.h"


Quad::ChildWindowDockingSystem::ChildWindowDockingSystem(ESystemID systemID)
	:WindowDockingSystem(systemID,"ChildWindowDockingSystem")
{
}

void Quad::ChildWindowDockingSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
{
	WindowDockingSystem::Initialize(clientWidth, clientHeight, map);


	Map* currentMap = GetMap();
	int mapLayerIndex = 0;
//	DockingSelectBox* leftDockingSelectBox = new DockingSelectBox("LeftDockingSelectBox", EDockingDirection::eLeft);
	//DockingSelectBox* leftDockingSelectBox = static_cast<DockingSelectBox*>(currentMap->CreateObject("DockingSelectBox"));
	DockingSelectBox* leftDockingSelectBox = DockingSelectBox::Create(currentMap, mapLayerIndex, EDockingDirection::eLeft);
	DockingSelectBox* rightDockingSelectBox = DockingSelectBox::Create(currentMap, mapLayerIndex, EDockingDirection::eRight);
	DockingSelectBox* topDockingSelectBox = DockingSelectBox::Create(currentMap, mapLayerIndex, EDockingDirection::eTop);
	DockingSelectBox* bottomDockingSelectBox = DockingSelectBox::Create(currentMap, mapLayerIndex, EDockingDirection::eBottom);


	/*DockingSelectBox* rightDockingSelectBox = new DockingSelectBox("RightDockingSelectBox", EDockingDirection::eRight);
	DockingSelectBox* topDockingSelectBox = new DockingSelectBox("TopDockingSelectBox", EDockingDirection::eTop);
	DockingSelectBox* bottomDockingSelectBox = new DockingSelectBox("BottomDockingSelectBox", EDockingDirection::eBottom);*/


	//mDockingPanel = new DockingPanel("DockingPanel");
//	mDockingPanel = static_cast<DockingPanel*>(currentMap->CreateObject("DockingPanel"));
	mDockingPanel = DockingPanel::Create(currentMap, mapLayerIndex);

	//leftDockingSelectBox->SetSystem(this);
	//rightDockingSelectBox->SetSystem(this);
	//topDockingSelectBox->SetSystem(this);
	//bottomDockingSelectBox->SetSystem(this);
	//mDockingPanel->SetSystem(this);



	//leftDockingSelectBox->Initialize(EDockingDirection::eLeft);
	//rightDockingSelectBox->Initialize(EDockingDirection::eRight);
	//topDockingSelectBox->Initialize(EDockingDirection::eTop);
	//bottomDockingSelectBox->Initialize(EDockingDirection::eBottom);
	//mDockingPanel->Initialize();

	float dockingSelectBoxWidth = 50.0f;
	float dockingSelectBoxHeight = 20.0f;
	leftDockingSelectBox->SetSize(dockingSelectBoxWidth, dockingSelectBoxHeight);
	rightDockingSelectBox->SetSize(dockingSelectBoxWidth, dockingSelectBoxHeight);
	topDockingSelectBox->SetSize(dockingSelectBoxWidth, dockingSelectBoxHeight);
	bottomDockingSelectBox->SetSize(dockingSelectBoxWidth, dockingSelectBoxHeight);

	leftDockingSelectBox->SetRotataion(90.0f);
	rightDockingSelectBox->SetRotataion(90.0f);


	leftDockingSelectBox->SetPosition(-30.0f, 0, 1);
	rightDockingSelectBox->SetPosition(+30.0f, 0, 1);
	topDockingSelectBox->SetPosition(0.0f, 50.0f, 1);
	bottomDockingSelectBox->SetPosition(0.0f, -50.0f, 1);


	leftDockingSelectBox->GetModel()->GetUiColliderComponent()->GetCollider()->Update(0.0f);
	rightDockingSelectBox->GetModel()->GetUiColliderComponent()->GetCollider()->Update(0.0f);
	topDockingSelectBox->GetModel()->GetUiColliderComponent()->GetCollider()->Update(0.0f);
	bottomDockingSelectBox->GetModel()->GetUiColliderComponent()->GetCollider()->Update(0.0f);
	mDockingPanel->GetModel()->GetUiColliderComponent()->GetCollider()->Update(0.0f);

	//add에서 system셋팅도괜찮을까.
//	AddEntity(leftDockingSelectBox);
//	AddEntity(rightDockingSelectBox);
//	AddEntity(topDockingSelectBox);
//	AddEntity(bottomDockingSelectBox);
//	AddEntity(mDockingPanel);


	mDockingPanel->SetActiveFlag(false);
	mDockingPanel->SetDrawFlag(true);



}

void Quad::ChildWindowDockingSystem::Update(float deltaTime, bool playMode)
{
	WindowDockingSystem::Update(deltaTime,playMode);

}

void Quad::ChildWindowDockingSystem::OnResize(UINT clientWidth, UINT clientHeight)
{
	WindowDockingSystem::OnResize(clientWidth, clientHeight);
}

void Quad::ChildWindowDockingSystem::AddEntity(Object* entity, int mapLayerID, bool engineEditObject)
{
	//GetMap()->AddObject(entity,mapLayerID);

}

//void Quad::ChildWindowDockingSystem::GetEntity(std::vector<Object*>& oEntityVector)
//{
//	GetMap()->GetObjectVectorInViewFrustum(oEntityVector);
//
//}



void Quad::ChildWindowDockingSystem::HandleEvent(Event* event)
{
	WindowDockingSystem::HandleEvent(event);

	const std::string& eventName = event->GetName();


}

//const std::vector<Quad::MapLayer>& Quad::ChildWindowDockingSystem::GetEntity()
//{
//	// TODO: 여기에 return 문을 삽입합니다.
//}




void Quad::ChildWindowDockingSystem::SelectBoxHover(EDockingDirection boxDirection)
{
	//방향에따라,창의크기에따라  반투명판의 크기,위치를 조정하고,
	//반투명판을 렌더링하도록설정
	using dir = EDockingDirection;


	float panelWidth = 0;
	float panelHeight = 0;
	//위치
	float panelPositionX = 0;
	float panelPositionY = 0;
	switch (boxDirection)
	{

	case dir::eLeft:

		panelWidth = GetClientWidth() / 2.0f;
		panelHeight = GetClientHeight();

		panelPositionX = -1.0f * panelWidth / 2.0f;
		panelPositionY = 0.0f;

		break;
	case dir::eRight:

		panelWidth = GetClientWidth() / 2.0f;
		panelHeight = GetClientHeight();

		panelPositionX = 1.0f * panelWidth / 2.0f;
		panelPositionY = 0.0f;
		break;
	case dir::eTop:
		panelWidth = GetClientWidth();
		panelHeight = GetClientHeight() / 2.0f;

		panelPositionX = 0.0f;
		panelPositionY = panelHeight / 2.0f;
		break;
	case dir::eBottom:
		panelWidth = GetClientWidth();
		panelHeight = GetClientHeight() / 2.0f;

		panelPositionX = 0.0f;
		panelPositionY = -1.0f * panelHeight / 2.0f;

		break;
	}

	mDockingPanel->SetPosition(panelPositionX, panelPositionY, 1.0f);
	mDockingPanel->SetSize(panelWidth, panelHeight);
	mDockingPanel->SetDrawFlag(true);
	mDockingPanel->SetActiveFlag(true);



}

void Quad::ChildWindowDockingSystem::SelectBoxReleaseHover()
{

	mDockingPanel->SetDrawFlag(false);
	mDockingPanel->SetActiveFlag(false);


}

void Quad::ChildWindowDockingSystem::SelectBoxLButtonUp(EDockingDirection boxDirection,bool state)
{

	//프레임윈도우에게 윈도우의 도킹처리를 해달라고해야한다.

	//현재 프레임윈도우가 가지는 movingwindow가 이 윈도우의 다음방향으로 도킹되어야한다라고

	SetDockingSystemState(false);
	mDockingPanel->SetDrawFlag(false);
	mDockingPanel->SetActiveFlag(false);

	if (state)
	{
	
		FrameWindowController::SetDockingChildWindow(static_cast<TaskWindow*>(GetWindow()), boxDirection);
	}
	//컨트롤러가 null이라고?

}

void Quad::ChildWindowDockingSystem::SetDockingSystemState(bool state)
{
	WindowDockingSystem::SetDockingSystemState(state);
	/*if (state == true)
	{
		mLeftDockingSelectBox
		mRightDockingSelectBox 
		mTopDockingSelectBox 
		mBottomDockingSelectBox


	}*/

}

