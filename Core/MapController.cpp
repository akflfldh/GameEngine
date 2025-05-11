#include "MapController.h"
namespace Quad
{
	MapController::MapController()
	{
	}

	void MapController::Initialize(RenderSystem* renderSystem, MeshManager* meshManager, MapManager* mapManager)
	{
		mRenderSystem = renderSystem;
		mMeshManager = meshManager;
		mMapManager = mapManager;
		mEventQueue.Initialize(100);

		CreateDefaultMap();
		mCurrentMap = mMapManager->GetMap("default");

	}

	void MapController::AddEvent(Event* event)
	{
		WriterStart();

		if (!mEventQueue.isFull())
			mEventQueue.PushBack(event);


		WriterEnd();
	}

	void MapController::CreateDefaultMap()
	{
		/*Map * defaultMap= mMapManager->CreateMap("default");

		Entity* baseRect = new Entity();
		Mesh * defaultMesh= mMeshManager->GetMesh("default");

		baseRect->Initialize("baseRect", defaultMesh);
		defaultMap->AddObject(baseRect);*/

	}

	void MapController::CreateObject()
	{
	}

	bool MapController::GetEvent(Event* oEvent)
	{

		WriterStart();

		if (!mEventQueue.isEmpty())
		{
			oEvent = mEventQueue.Front();
			mEventQueue.Pop();

			WriterEnd();
			return true;
		}

		WriterEnd();
		return false;
	}


}