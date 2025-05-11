//#include "System/AttributeSystem.h"
//#include"EditorDirector/DefaultState.h"
//#include"EditorDirector/DockingWindowController.h"
//
//Quad::AttributeSystem::AttributeSystem():
//	System(ESystemType::eMainSystem,ESystemID::eAttributeMainSystem,"AttributeSystem")
//{
//}
//
//void Quad::AttributeSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
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
//void Quad::AttributeSystem::Update(float deltaTime, bool playMode)
//{
//	System::Update(deltaTime,playMode);
//
//}
//
//void Quad::AttributeSystem::OnResize(UINT clientWidth, UINT clientHeight)
//{
//	System::OnResize(clientWidth, clientHeight);
//	D3D12_VIEWPORT viewport = GetViewPort();
//	viewport.TopLeftY = ((DockingWindowController*)GetController())->GetTitleBarHeight();
//	SetViewPort(viewport.TopLeftX, viewport.TopLeftY, viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth);
//}
//
//void Quad::AttributeSystem::AddEntity(Object* entity, int mapLayerID, bool engineEditObject)
//{
//	
//
//}
