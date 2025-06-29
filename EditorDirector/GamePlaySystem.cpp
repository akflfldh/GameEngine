//#include "System/GamePlaySystem.h"
//#include"Object/Entity.h"
//#include"GamePlaySystemDefaultState.h"
//#include"DockingWindowController.h"
//#include"Gizmo.h"
//#include"ResourceManager/MeshManager.h"
//
//namespace Quad
//{
//	GamePlaySystem::GamePlaySystem()
//		:System(ESystemType::eMainSystem,ESystemID::eGamePlayMainSystem,"GamePlaySystem",Quad::ESystemMode::eSwitchMode)
//	{
//
//	}
//
//	void GamePlaySystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* staticMap, Quad::Map* runtimeMap)
//	{
//	//	SetInitSetting(true);
//
//		System::Initialize(clientWidth, clientHeight, staticMap,runtimeMap);
//		//SetPlayModeState(false);
//
//
//		SetSystemState(new GamePlaySystemDefaultState("Default"));
//		FixTopLeftGlobalX(0.0f);
//		FixTopLeftGlobalY(40.0f);
//
//	}
//
//	void GamePlaySystem::Start()
//	{
//		System::Start();
//	}
//
//	void GamePlaySystem::Update(float deltaTime, bool playMode)
//	{
//
//		System::Update(deltaTime,playMode);
//
//	}
//
//	void GamePlaySystem::OnResize(UINT clientWidth, UINT clientHeight)
//	{
//		System::OnResize(clientWidth, clientHeight); 
//
//		//D3D12_VIEWPORT viewport = GetViewPort();
//	//	viewport.TopLeftY = ((DockingWindowController*)GetController())->GetTitleBarHeight();
//	//	SetViewPort(viewport.TopLeftX,viewport.TopLeftY,viewport.Width,viewport.Height,viewport.MinDepth,viewport.MaxDepth);
//
//	}
//
//	void GamePlaySystem::AddEntity(Object* entity, int mapLayerID, bool engineEditObject)
//	{
//
//		//GetMap()->AddObject(entity, mapLayerID);
//
//	}
//
//	/*void GamePlaySystem::GetEntity(std::vector<Object*>& entityVector)
//	{
//		Map* map = GetMap();
//		map->GetObjectVectorInViewFrustum(entityVector);
//
//
//	}*/
//
//	//const std::vector<MapLayer>& GamePlaySystem::GetEntity()
//	//{
//	//	// TODO: 여기에 return 문을 삽입합니다.
//	//}
//
//
//
//
//}