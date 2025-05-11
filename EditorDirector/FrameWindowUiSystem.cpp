#include "System/FrameWindowUiSystem.h"
#include"Map/Map.h"
#include"Object/Entity.h"
#include"DefaultState.h"
#include"ContentItemUiPanelEntity.h"
#include"Controller.h"
#include"DockingWindowController.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"ContentIProjecttemUiEntity.h"
#include"ProjectMetaData.h"

#include"ButtonUiEntitiy.h"

#include"TextBox.h"
#include"TextBoxPanel.h"
#include"CreateProjectFolderHierarchy.h"

//#include"CreatingProjectDirector.h"


Quad::FrameWindowUiSystem::FrameWindowUiSystem()
	:System(ESystemType::eMainSystem,ESystemID::eFrameMainSystem,"FrameWindowUiSystem")
{


}

Quad::FrameWindowUiSystem::~FrameWindowUiSystem()
{
}

void Quad::FrameWindowUiSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
{
	System::Initialize(clientWidth, clientHeight, map);
	SetSystemState(new DefaultState("Default"));
	OnRuntimeMode();
	//SetPlayModeState(true);



	DockingWindowController* windowController = static_cast<DockingWindowController*>(GetController());
	


}

void Quad::FrameWindowUiSystem::Update(float deltaTime, bool playMode)
{
	System::Update(deltaTime,playMode);

}

void Quad::FrameWindowUiSystem::OnResize(UINT clientWidth, UINT clientHeight)
{
	System::OnResize(clientWidth, clientHeight);

}

void Quad::FrameWindowUiSystem::AddEntity(Object* entity, int mapLayerID, bool engineEditObject)
{
	Map* map = GetMap();
//	map->AddObject(entity,mapLayerID);


}

//void Quad::FrameWindowUiSystem::GetEntity(std::vector<Object*>& oEntityVector)
//{
//
//	Map* map = GetMap();
//	map->GetObjectVectorInViewFrustum(oEntityVector);
//	
//
//
//}

//const std::vector<Quad::MapLayer>& Quad::FrameWindowUiSystem::GetEntity()
//{
//	// TODO: 여기에 return 문을 삽입합니다.
//}

void Quad::FrameWindowUiSystem::HandleEvent(Event* pEvent)
{
	System::HandleEvent(pEvent);


}

