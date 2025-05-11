#include "System/FileUiSystem.h"
#include"Object/Entity.h"
#include"Core/Event.h"
#include"EditorDirector/DefaultState.h"
#include"EditorDirector/DockingWindowController.h"

Quad::FileUiSystem::FileUiSystem()
	:System(ESystemType::eMainSystem,ESystemID::eFileUiMainSystem,"FileUiSystem")
{

}

Quad::FileUiSystem::~FileUiSystem()
{

}

void Quad::FileUiSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
{
 	System::Initialize(clientWidth, clientHeight, map);
	//SetPlayModeState(true);
	SetSystemState(new DefaultState("Default"));
	OnRuntimeMode();


}

void Quad::FileUiSystem::Update(float deltaTime,bool playMode) 
{
	System::Update(deltaTime,playMode);
}

void Quad::FileUiSystem::OnResize(UINT clientWidth, UINT clientHeight)
{
	System::OnResize(clientWidth, clientHeight);
	D3D12_VIEWPORT viewport=GetViewPort();
	viewport.TopLeftY = ((DockingWindowController*)GetController())->GetTitleBarHeight();
	SetViewPort(viewport.TopLeftX, viewport.TopLeftY, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth);
}

void Quad::FileUiSystem::AddEntity(Object* entity, int mapLayerID, bool engineEditObject)
{
	Map * map =	GetMap();
	//map->AddObject(entity,mapLayerID);

}

//void Quad::FileUiSystem::GetEntity(std::vector<Object*>& oEntityVector)
//{
//
//	Map* map = GetMap();
//	map->GetObjectVectorInViewFrustum(oEntityVector);
//
//}
////
//const std::vector<Quad::MapLayer>& Quad::FileUiSystem::GetEntity()
//{
//	// TODO: 여기에 return 문을 삽입합니다.
//}

void Quad::FileUiSystem::HandleEvent(Event* event)
{
	System::HandleEvent(event);


	const std::string& eventName = event->GetName();



}
