#include "System/DragAndDropSystem.h"
#include"Object/Entity.h"

Quad::DragAndDropSystem::DragAndDropSystem()
	:System(ESystemType::eMainSystem,ESystemID::eDragAndDropMainSystem,"DragAndDropSystem")
{


}

void Quad::DragAndDropSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
{

	System::Initialize(clientWidth, clientHeight, map);
	OnRuntimeMode();
	//SetPlayModeState(true);
}

void Quad::DragAndDropSystem::Update(float deltaTime, bool playMode)
{
	System::Update(deltaTime,playMode);

}

void Quad::DragAndDropSystem::OnResize(UINT clientWidth, UINT clientHeight)
{
	System::OnResize(clientWidth, clientHeight);
}

void Quad::DragAndDropSystem::GetRenderSettingItem(RenderSettingItem& oRenderSettingItem) const
{
	System::GetRenderSettingItem(oRenderSettingItem);

}

void Quad::DragAndDropSystem::AddEntity(Object* entity, int mapLayerID, bool engineEditObject)
{
	Map* map = GetMap();
//	map->AddObject(entity,mapLayerID);


}

//void Quad::DragAndDropSystem::GetEntity(std::vector<Object*>& oEntityVector)
//{
//	Map* map = GetMap();
//	map->GetObjectVectorInViewFrustum(oEntityVector);
//
//}

//const std::vector<Quad::MapLayer>& Quad::DragAndDropSystem::GetEntity()
//{
//	// TODO: 여기에 return 문을 삽입합니다.
//}

void Quad::DragAndDropSystem::HandleEvent(Event* event)
{
	System::HandleEvent(event);


}
