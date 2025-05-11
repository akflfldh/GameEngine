#include "System/WindowDockingSystem.h"
#include"Object/Entity.h"
#include"EditorDirector/DockingPanel.h"
#include"EditorDirector/DefaultState.h"
#include"EditorDirector/FrameWindowController.h"
#include"EditorDirector/SizeControllerStickEntity.h"
#include"EditorDirector/DockingSelectBox.h"


Quad::WindowDockingSystem::WindowDockingSystem(ESystemID systemID, const std::string& name)
	:System(ESystemType::eDockingSystem, systemID,name)
{


}

void Quad::WindowDockingSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map  )
{
	System::Initialize(clientWidth, clientHeight, map);
	SetSystemState(new DefaultState("Default"));
	OnRuntimeMode();
	//SetPlayModeState(true);


}

void Quad::WindowDockingSystem::Update(float deltaTime, bool playMode)
{
	System::Update(deltaTime,playMode);
}

void Quad::WindowDockingSystem::OnResize(UINT clientWidth, UINT clientHeight)
{
	System::OnResize(clientWidth, clientHeight);

}

void Quad::WindowDockingSystem:: AddEntity(Object* entity, int mapLayerID, bool engineEditObject)
{
	Map* map = GetMap();
	//map->AddObject(entity, mapLayerID);

}

//void Quad::WindowDockingSystem::GetEntity(std::vector<Object*>& oEntityVector)
//{
//	Map* map = GetMap();
//	map->GetObjectVectorInViewFrustum(oEntityVector);
//
//}

void Quad::WindowDockingSystem::HandleEvent(Event* event)
{
	if(GetActiveState())
		System::HandleEvent(event);

}
//
//const std::vector<Quad::MapLayer>& Quad::WindowDockingSystem::GetEntity()
//{
//	// TODO: 여기에 return 문을 삽입합니다.
//}

void Quad::WindowDockingSystem::SetDockingSystemState(bool state)
{
	mDockingSystemState = state;


}

bool Quad::WindowDockingSystem::GetDockingSystemState() const
{
	return mDockingSystemState;
}


