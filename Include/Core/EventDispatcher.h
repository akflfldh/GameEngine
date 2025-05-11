#pragma once




#include"Predeclare.h"
#include"Utility/Singleton.h"
#include<unordered_map>
#include"System.h"
#include"Core/CoreDllExport.h"


namespace Quad
{
	class CORE_API_LIB EventDispatcher :public Singleton<EventDispatcher>
	{
	public:
		EventDispatcher() = default;
		~EventDispatcher() = default;

		void Initialize();

		//엔진에서는 여기에 모든시스템을 등록하면되고 게임에서는 게임플레이윈도우들의 시스템만등록하면될것이다.
		void RegisterSystem(System* system);
	

		static void SendEvent(Event* event, ESystemID systemID);

		static void SendSelectEvent(Object* destObject, bool state, bool exclusiveEventHandleFlag);


	private:
		std::unordered_map<ESystemID, System*> mSystemUnMapTable;


	};

}
