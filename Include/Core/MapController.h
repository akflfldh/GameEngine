#pragma once


#include"Controller.h"

#include"MapManager.h"
#include"RenderSystem.h"
#include"ResourceManager/MeshManager.h"
#include"Object/Entity.h"
#include"CircleQueue.h"
#include"ReaderWriter.h"
#include"Event.h"
#include"Core/CoreDllExport.h"
namespace Quad
{


	class CORE_API_LIB MapController :public Controller, public ReaderWriter
	{
	public:
		MapController();
		~MapController() = default;

		void Initialize(RenderSystem* renderSystem, MeshManager* meshManager, MapManager* mapManager);


		void AddEvent(Event* event);
		//readerStart, WriterStart,WriterEnd함수를 호출하여 이 함수를 둘러쌀것
		//아니다 그냥 AddEvent함수안에서 호출할것 그럼 외부에서 신경쓸이유가없다.


	private:
		void CreateDefaultMap();
		void CreateObject();


		bool GetEvent(Event* oEvent);

	private:
		RenderSystem* mRenderSystem;
		MeshManager* mMeshManager;
		MapManager* mMapManager;

		Quad::Map* mCurrentMap;

		CircleQueue<Event*> mEventQueue;


	};

}

