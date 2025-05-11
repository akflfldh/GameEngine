#include "System/PopupWindowUiSystem.h"
#include"PanelUiEntity.h"
#include"PopupPanelID.h"
#include"Object/Camera/Camera.h"
#include"Object/Camera/OrthogoanlCamera.h"
#include"DefaultState.h"

Quad::PopupWindowUiSystem::PopupWindowUiSystem()
	:System(ESystemType::eMainSystem,ESystemID::ePopupUiSystem,"PopipWindowUiSystem"), mRootPanelVector(255,nullptr)
{
}

Quad::PopupWindowUiSystem::~PopupWindowUiSystem()
{
}

void Quad::PopupWindowUiSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
{	
	DefaultState* defaultState = new DefaultState("");
	SetSystemState(defaultState);

	System::Initialize(clientWidth, clientHeight, map);
	OnRuntimeMode();
	//SetPlayModeState(true);
	CreateFileUiPanel();

	mCurrentPopupPanelID = -1;

	
	SetActiveState(true);

}

void Quad::PopupWindowUiSystem::Update(float deltaTime, bool playMode)
{
	System::Update(deltaTime,playMode);

}

void Quad::PopupWindowUiSystem::OnResize(UINT clientWidth, UINT clientHeight)
{
	System::OnResize(clientWidth, clientHeight);

}

void Quad::PopupWindowUiSystem::AddEntity(Object* object, int mapLayerID, bool engineEditObject)
{
	Map* map = GetMap();
//	map->AddObject(object,mapLayerID);


}

//void Quad::PopupWindowUiSystem::GetEntity(std::vector<Object*>& oObjectVector)
//{
//
//	Map* map = GetMap();
//	map->GetObjectVectorInViewFrustum(oObjectVector);
//
//}

void Quad::PopupWindowUiSystem::HandleEvent(Event* event)
{
	System::HandleEvent(event);


	


}

//const std::vector<Quad::MapLayer>& Quad::PopupWindowUiSystem::GetEntity()
//{
//	// TODO: 여기에 return 문을 삽입합니다.
//}

void Quad::PopupWindowUiSystem::ActivatePanel(int popupPanelID)
{
	
	if (mCurrentPopupPanelID != -1)
	{
		
		mRootPanelVector[mCurrentPopupPanelID]->SetActiveFlag(false);
		mRootPanelVector[mCurrentPopupPanelID]->SetDrawFlag(false);
		mRootPanelVector[mCurrentPopupPanelID]->SetSelectAvailableFlag(false);
	}

	mCurrentPopupPanelID = popupPanelID;
	mRootPanelVector[popupPanelID]->SetActiveFlag(true);
	mRootPanelVector[popupPanelID]->SetDrawFlag(true);
	mRootPanelVector[popupPanelID]->GetTransform().SetDirtyFlag(true);
	mRootPanelVector[popupPanelID]->SetSelectAvailableFlag(true);

	Map * map =	GetMap();

	float rootPanelWidth =	mRootPanelVector[popupPanelID]->GetWidth();
	float rootPanelHeight = mRootPanelVector[popupPanelID]->GetHeight();
	//(직교투영)카메라의 시야박스의 width,height도 변경
	SetViewPort(0, 0, rootPanelWidth, rootPanelHeight,0.0f, 1.0f);

	OrthogoanlCamera* camera =static_cast<OrthogoanlCamera*>(map->GetMainCamera());
	camera->SetViewWidthAndHeight(rootPanelWidth, rootPanelHeight);

}

std::pair<int, int> Quad::PopupWindowUiSystem::GetPopupPanelSize(int popupPanelID) const
{
	std::pair<int, int> size;

	size.first=	mRootPanelVector[popupPanelID]->GetWidth();
	size.second=mRootPanelVector[popupPanelID]->GetHeight();

	return size;
}

void Quad::PopupWindowUiSystem::RegisterRootPanel(int popupPanelID, PanelUiEntity* rootPanel)
{

	mRootPanelVector[popupPanelID] = rootPanel;

}

void Quad::PopupWindowUiSystem::CreateFileUiPanel()
{

	Map* map = GetMap();
	int mapLayerIndex = 0;
	//background panel
//	PanelUiEntity* rootPanel = new PanelUiEntity("FileUiRootPopupPanel");
	//PanelUiEntity* rootPanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
	PanelUiEntity* rootPanel = PanelUiEntity::Create(map, mapLayerIndex);

	//rootPanel->SetSystem(this);
	rootPanel->Initialize();
	rootPanel->SetSize(300, 600);
	float width = rootPanel->GetWidth();
	float height= rootPanel->GetHeight();

	rootPanel->SetPosition(width / 2, -1.0f * height / 2, 10);


	rootPanel->SetSelectAvailableFlag(false);
	rootPanel->SetDrawFlag(false);

	mRootPanelVector[FILEUIPOPUP] = rootPanel;

}
