//#include "System/FrameWindowSystem.h"
//#include"Map/Map.h"
//#include"Object/Entity.h"
//#include"EditorDirector/DefaultState.h"
//
//Quad::FrameWindowSystem::FrameWindowSystem()
//	:System(ESystemType::eMainSystem,ESystemID::eFrameMainSystem,"FrameWindowSystem")
//{
//
//
//
//}
//
//Quad::FrameWindowSystem::~FrameWindowSystem()
//{
//}
//
//void Quad::FrameWindowSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
//{
//	System::Initialize(clientWidth, clientHeight, map);
//	//SetPlayModeState(true);
//	SetSystemState(new DefaultState("Default"));
//	OnRuntimeMode();
//
//
//
//}
//
//void Quad::FrameWindowSystem::Update(float deltaTime, bool playMode)
//{
//	System::Update(deltaTime,playMode);
//
//}
//
//void Quad::FrameWindowSystem::OnResize(UINT clientWidth, UINT clientHeight)
//{
//	System::Update(0.0f);
//
//}
//
//void Quad::FrameWindowSystem::AddEntity(Object* entity, int mapLayerID, bool engineEditObject)
//{
//	Map* map = GetMap();
////	map->AddObject(entity,mapLayerID);
//
//}
//
////void Quad::FrameWindowSystem::GetEntity(std::vector<Object*>& oEntityVector)
////{
////	Map* map = GetMap();
////	map->GetObjectVectorInViewFrustum(oEntityVector);
////
////}
//
//void Quad::FrameWindowSystem::HandleEvent(Event* pEvent)
//{
//	System::HandleEvent(pEvent);
//
//
//}
//
////const std::vector<Quad::MapLayer>& Quad::FrameWindowSystem::GetEntity()
////{
////	// TODO: 여기에 return 문을 삽입합니다.
////}
