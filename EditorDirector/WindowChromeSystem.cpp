#include "System/WindowChromeSystem.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"Object/UiEntity.h"
#include"TitleBarEntity.h"
#include"DefaultState.h"
#include"SizeControllerStickEntity.h"
#include"DockingWindowController.h"
#include"Window.h"
#include"ObjectManager/ObjectManager.h"

Quad::WindowChromeSystem::WindowChromeSystem(ESystemID systemID)
	:System(ESystemType::eWindowLayoutSystem, systemID,"WindowChromeSystem",Quad::ESystemMode::eRunTimeMode), mTitleBarId(0), mTitleBar(nullptr)
{
}

void Quad::WindowChromeSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
{
	System::Initialize(clientWidth,clientHeight,map);
	SetSystemState(new DefaultState("Default"));
	OnRuntimeMode();
	//SetPlayModeState(true);



	//이 레이아웃에 관련된 엔티티들은 그냥바로 w가 1.0이고 동차절단공간에있도록하면 될거같다.

	//Map* map = GetMap();

	//TitleBarEntity* titleBar = new TitleBarEntity;

	Map * currMap =	GetMap();
	int mapLayerIndex = 0;
	BaseObjectManager* objectManager = EditObjectManager::GetInstance();

	//TitleBarEntity* titleBar = static_cast<TitleBarEntity*>(currMap->CreateObject("TitleBarEntity"));
	TitleBarEntity* titleBar = TitleBarEntity::Create(currMap, mapLayerIndex);

	mTitleBar = titleBar;
	//titleBar->SetSystem(this);
	//titleBar->Initialize();
	mTitleBarId = titleBar->GetUniqueID();
	//titleBar->SetSystem(this);
	mTitleBarHeight= titleBar->GetHeight();

	//map->AddObject(titleBar);

}

void Quad::WindowChromeSystem::Update(float deltaTime, bool playMode)
{
	System::Update(deltaTime,playMode);

}

void Quad::WindowChromeSystem::OnResize(UINT clientWidth, UINT clientHeight)
{
	System::OnResize(clientWidth,clientHeight);
	
	int a = 2;
	//Update();

}

void Quad::WindowChromeSystem::AddEntity(Object* entity, int mapLayerID, bool engineEditObject)
{
	Map* map = GetMap();
	//map->AddObject(entity, mapLayerID);


}

//void Quad::WindowChromeSystem::GetEntity(std::vector<Object*>& oEntityVector)
//{
//	Map* map = GetMap();
//
//	map->GetObjectVectorInViewFrustum(oEntityVector);
//
//
//
//}

//const std::vector<Quad::MapLayer>& Quad::WindowChromeSystem::GetEntity()
//{
//	// TODO: 여기에 return 문을 삽입합니다.
//}

void Quad::WindowChromeSystem::SetTitleBarSize(int width ,int height)
{
	mTitleBarHeight = height;
	Map* map = GetMap();
	WindowControlEntity * titleBar  =  (WindowControlEntity*) map->FindObject(mTitleBarId);
	//map에서 object를 탐색할수있어야하고


	DockingWindowController* controller =(DockingWindowController *)GetController();
	TaskWindow* window = controller->GetWindow();
	
	titleBar->SetSize(width, height);

	


}

int Quad::WindowChromeSystem::GetTitleBarHeight() const
{
	return mTitleBarHeight;
}

Quad::TitleBarEntity* Quad::WindowChromeSystem::GetTitleBarEntity() const
{
	return mTitleBar;
}



