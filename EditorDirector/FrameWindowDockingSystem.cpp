#include "System/FrameWindowDockingSystem.h"
#include"EditorDirector/SizeControllerStickEntity.h"


const int Quad::FrameWindowDockingSystem::mStickMaxNum;

Quad::FrameWindowDockingSystem::FrameWindowDockingSystem()
	:WindowDockingSystem(ESystemID::eFrameDockingSystem,"FrameWindowDockingSystem")
{

}

void Quad::FrameWindowDockingSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
{
	WindowDockingSystem::Initialize(clientWidth, clientHeight, map);

	Map* currMap = GetMap();


	int mapLayerIndex = 0;

	for (int i = 0; i < mStickArray.size(); ++i)
	{
		//mStickArray[i]=new SizeControllerStickEntity("Quad_SizeControllerSitckEntity_Num"+std::to_string(i));
	//	mStickArray[i] = static_cast<SizeControllerStickEntity*>(currMap->CreateObject("SizeControllerStickEntity"));
		mStickArray[i] = SizeControllerStickEntity::Create(currMap, mapLayerIndex);
		//mStickArray[i]=new SizeControllerStickEntity(L"");
		//mStickArray[i]->SetSystem(this);
	//	mStickArray[i]->Initialize();
		mStickArray[i]->SetDrawFlag(false);
		mStickArray[i]->SetActiveFlag(false);
	//	GetMap()->AddObject(mStickArray[i]);
	}



}

void Quad::FrameWindowDockingSystem::Update(float deltaTime, bool playMode)
{
	WindowDockingSystem::Update(deltaTime,playMode);
	
}

void Quad::FrameWindowDockingSystem::OnResize(UINT clientWidth, UINT clientHeight)
{
	WindowDockingSystem::OnResize(clientWidth, clientHeight);
}

void Quad::FrameWindowDockingSystem::AddEntity(Object* entity, int mapLayerID, bool engineEditObject)
{
	//GetMap()->AddObject(entity,mapLayerID);

}

//void Quad::FrameWindowDockingSystem::GetEntity(std::vector<Object*>& oEntityVector)
//{
//	GetMap()->GetObjectVectorInViewFrustum(oEntityVector);
//
//}



void Quad::FrameWindowDockingSystem::HandleEvent(Event* event)
{
	const std::string& eventName = event->GetName();


	WindowDockingSystem::HandleEvent(event);

	

}

//const std::vector<Quad::MapLayer>& Quad::FrameWindowDockingSystem::GetEntity()
//{
//	// TODO: 여기에 return 문을 삽입합니다.
//}

Quad::SizeControllerStickEntity* Quad::FrameWindowDockingSystem::CreateSizeControlStick()
{
	for (int i = 0; i < mStickArray.size(); ++i)
	{
		
		if (!mStickArray[i]->GetActiveFlag())
		{
			mStickArray[i]->SetActiveFlag(true);
			mStickArray[i]->SetDrawFlag(true);
			return mStickArray[i];
		}
	}

	return nullptr;
}

void Quad::FrameWindowDockingSystem::ReleaseSizeControlStick(SizeControllerStickEntity* stick)
{

	for (int i = 0; i < mStickArray.size(); ++i)
	{
		if (mStickArray[i] == stick)
		{
			mStickArray[i]->SetActiveFlag(false);
			mStickArray[i]->SetDrawFlag(false);
		}
	}


}
