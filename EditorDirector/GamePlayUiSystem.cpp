//#include "System/GamePlayUiSystem.h"
//#include"ButtonUiEntitiy.h"
//#include"DefaultState.h"
//#include"DockingWindowController.h"
//#include"Collision/CollisionWorld.h"
//
//namespace Quad
//{
//	GamePlayUiSystem::GamePlayUiSystem()
//		:UiSystem(ESystemID::eGamePlayUiSystem,"GamePlayUiSystem")
//	{
//		int a = 2;
//	}
//	void GamePlayUiSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
//	{
//		SetInitSetting(true);
//		
//		UiSystem::Initialize(clientWidth, clientHeight, map);
//	
//		//SetPlayModeState(false);
//		SetSystemState(new DefaultState("Default"));
//	
//
//
//
//	}
//
//	void GamePlayUiSystem::Update(float deltaTime, bool playMode)
//	{
//		UiSystem::Update(deltaTime,playMode);
//		D3D12_VIEWPORT viewport = GetViewPort();
//		viewport.TopLeftY = ((DockingWindowController*)GetController())->GetTitleBarHeight();
//		SetViewPort(viewport.TopLeftX, viewport.TopLeftY, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth);
//
//	}
//
//	void GamePlayUiSystem::OnResize(UINT clientWidth, UINT clientHeight)
//	{
//		UiSystem::OnResize(clientWidth, clientHeight);
//
//	}
//
//	void GamePlayUiSystem::AddEntity(Object* entity, int mapLayerID, bool engineEditObject)
//	{
//		Quad::Map* map = GetMap();
//
//		//Transform& transform = entity->GetTransform();
//		//transform.SetPositionLocal({ 0,0,0 });
//
//
//	//	map->AddObject(entity, mapLayerID);
//
//
//
//	}
//
//	//void GamePlayUiSystem::GetEntity(std::vector<Object*>& oUiEntityVector)
//	//{
//
//	//	Quad::Map* map = GetMap();
//
//	//	//map->GetObjectVector(oUiEntityVector);
//	//	SceneGraph* sceneGraph = map->GetSceneGraph();
//	////	sceneGraph->GetAllObjectVector(oUiEntityVector);
//	//	map->GetObjectVectorInViewFrustum(oUiEntityVector);
//	//	//sceneGraph->GetRootChildObjectVector(oUiEntityVector);
//
//	//	return;
//
//	//}
//
//	//const std::vector<Quad::MapLayer>& GamePlayUiSystem::GetEntity()
//	//{
//	//	// TODO: 여기에 return 문을 삽입합니다.
//	//}
//
//
//
//
//
//
//
//}