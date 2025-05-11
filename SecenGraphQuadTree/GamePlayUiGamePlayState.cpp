#include "GamePlayUiGamePlayState.h"
#include"GamePlayUiSystem.h"
#include"EventTable.h"
#include"GamePlayUiGamePlaySelectState.h"
#include"System.h"

namespace Quad
{
	GamePlayUiGamePlayState::GamePlayUiGamePlayState(const std::string& name)
		:SystemState(name)
	{
	}

	void GamePlayUiGamePlayState::EnterState(System& system)
	{
	}

	void GamePlayUiGamePlayState::RunState(System& system)
	{
	}

	void GamePlayUiGamePlayState::ExitState(System& syetem)
	{
	}

	SystemState* GamePlayUiGamePlayState::HandleEvent(System& system, Event* event)
	{

		const std::string& eventName = event->GetName();
		if (eventName == "LButtonDown")
		{
			return LButtonDown(system, event);
		}
		else if (eventName == "LButtonUp")
		{

		}
		else if (eventName == "MouseMove")
		{

		}

		return nullptr;
	}

	SystemState* GamePlayUiGamePlayState::LButtonDown(System& system, Event* event)
	{

		//MouseEvent* pMouseEvent = (MouseEvent*)event;


		////그리고 선택된 이벤트를 선택된 오브젝트에게보낸다
		////(여기는 RAY가 필요하고)

		//MouseEvent mouseSelectEvent("MouseSelect");
		//mouseSelectEvent.SetClientPosition(pMouseEvent->GetClientPositionX(), pMouseEvent->GetClientPositionY());

		//Ray ray;
		//MathHelper::CalculateRay({ (float)pMouseEvent->GetClientPositionX(),(float)pMouseEvent->GetClientPositionY() },
		//	system.GetViewPort(), *system.GetMap()->GetGameCamera(), true, ray);


		////여기서 MouseClickSelectEvent를 생성해서 그 이벤트를 선택될수있는 오브젝트들에게 보낸다.
		////그리고 상태를 SelecteState로 바꾼다
		////만약 선택된게없다면 
		////마우스 클릭이벤트로 마우스 클릭이벤트를 받을수잇는 오브젝트들에게 모두 보낸다.

		////콜리전월드를 통할까 아니면 직접할까 , 콜리전월드를 통해서하되 플래그만설정해놓고 하면되기도하고 

		//
		//
		////충돌판정을하고 충돌된 엔티티를돌려줘 그리고 그 엔티티가 여기서 선택될수있는플래그가있는지확인하는거지










		//std::vector<Object*> selectObjectVector;
		//const EventTable * eventTable =system.GetEventTable();
		//
		//eventTable->GetObjectVector(mouseSelectEvent.GetName(), selectObjectVector);
		//float minT = FLT_MAX;
		//int minIndex = -1;
		//for (int i = 0; i < selectObjectVector.size(); ++i)
		//{
		//	float t = FLT_MAX;
		//	if (selectObjectVector[i]->GetTransform().GetBoundingVolume()->Intersect(ray, t))
		//	{
		//		if (minT > t)
		//		{
		//			minT = t;
		//			minIndex = i;
		//		}
		//	}
		//}



		//if (minIndex != -1)
		//{
		//	selectObjectVector[minIndex]->OnEvent(&mouseSelectEvent);

		//	//상태를 Object를 선택한상태로 전환

		//	GamePlayUiGamePlaySelectState* selectState = new GamePlayUiGamePlaySelectState("GamePlayUiGameSelectState");
		//	selectState->SetSelectObject(selectObjectVector[minIndex]);

		//	//선택한것은좋은데 선택하니깐 제거되는 오브젝트일수도있지 그러니깐 포인터는 위험하다.

		//	selectState->SetEventFocusFlag(true);
		//	//false이냐 이거는 고려

		//	return selectState;
		//}






		////클릭하는 이벤트는 모두에게 
		////(여기는 그냥 모두전달)

		//std::vector<Object*> objectVector;
		//eventTable->GetObjectVector("LButtonDown", objectVector);

		//for (int i = 0; i < objectVector.size(); ++i)
		//{
		//	objectVector[i]->OnEvent(event);
		//}

		////이벤트를 다 보내는것은 비효율적이지
		////이벤트마다 이 이벤트를 받을 오브젝트를 등록해서 유지하자.
		////유저에게 이 이벤트에 등록하게만드는거지 (물론 게임플레이시에유효한거지 )
		////(게임플레이중이아닐때는 엔진에서 설정하는거대로)



		//return nullptr;


		return nullptr;


	}

}