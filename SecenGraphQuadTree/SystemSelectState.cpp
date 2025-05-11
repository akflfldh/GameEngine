#include "SystemSelectState.h"
#include"UiEntity.h"
#include"DefaultState.h"
#include"EventTable.h"

#include"KeyBoard.h"
//#include"Text.h"
#include"GamePlayUiSystem.h"
#include"SpacePartitioningStructure.h"
#include"MouseClickSelectEvent.h"
#include"MouseClickState.h"
#include"System.h"
#include"Window.h"
#include"MouseInOutEvent.h"
#include"CollisionWorld.h"

namespace Quad
{

	Quad::SystemSelectState::SystemSelectState(const std::string& name)
		:SystemState(name)
	{
	}

	void Quad::SystemSelectState::EnterState(System& system)
	{
	}

	void Quad::SystemSelectState::RunState(System& system)
	{
	}

	void Quad::SystemSelectState::ExitState(System& syetem)
	{
	}

	void Quad::SystemSelectState::SetSelectedEntity(UiEntity* entity)
	{
		mSelectedEntity = entity;
	}

	Quad::UiEntity* Quad::SystemSelectState::GetSelectedEntity() const
	{
		return nullptr;
	}

	Quad::SystemState* Quad::SystemSelectState::HandleEvent(System& system, Event* event)
	{
		const std::string& eventName = event->GetName();
		if (eventName == "LButtonDown")
		{

			return HandleLButtonDown(system, event);

		}
		else if (eventName == "KeyDown")
		{
			return HandleKeyDown(system, event);
		}
		else if (eventName == "KeyUp")
		{
			return HandleKeyUp(system, event);
		}
		else if (eventName == "MouseMove")
		{
			return HandleMouseMove(system, event);
		}
		else if (eventName == "MouseInOut")
		{
			return HandleMouseInOut(system, event);
		}


	}

	SystemState* Quad::SystemSelectState::HandleKeyDown(System& syetem, Event* event)
	{
		std::vector<Object*> keyDownObjectVector;
		mSelectedEntity->OnEvent(event);
		//키를 눌렀는데 삭제됬다 그러면 이 엔티티참조는 어떻게할것인가 결국그게 뭐다 핸들 테이블을
		//사용하는것이 더 바람직할수있다.
		//관리면에서

		return nullptr;

	}

	SystemState* Quad::SystemSelectState::HandleKeyUp(System& syetem, Event* event)
	{

		std::vector<Object*> keyUpObjectVector;
		mSelectedEntity->OnEvent(event);

		return nullptr;

	}

	SystemState* SystemSelectState::HandleLButtonDown(System& system, Event* event)
	{
		//만약 다른곳을 눌렀다면 
			//디폴트상태또는 다른 엔티티를 누른상태(click state로 간다)

			//다시 ray판정을 수행
			//그리고는 다시 mouse click 으로 이동  (새로운엔티티이건 기존선택한 엔티티이건 클릭상태로 간다)
			//물론 전에선택고있던 엔티티일수도있고 아닐수도있고 아닌경우 ReleaseSelect이벤트를 보내준다.
			//새로운엔티티인경우 Select이벤트를 보내준다.

		MouseEvent* mouseEvent = (MouseEvent*)event;

		Map* map = system.GetMap();
	//	SpacePartitioningStructure* spacePartitioningStructure = map->GetSpaceParitioningStructure();
		CollisionWorld* collisionWorld = map->GetCollisionWorld();

		Object* object = nullptr;
		Ray ray;
		MathHelper::CalculateRay({ (float)mouseEvent->GetClientPositionX(), (float)mouseEvent->GetClientPositionY() }
		, system.GetViewPort(), *map->GetMainCamera(), true, ray);
		bool rayRet = collisionWorld->RayCastingFirst(object, ray);


		const EventTable* eventTable = system.GetEventTable();

		if (!rayRet)
		{
			// 판단은 괜찮은데 문제는 ui뿐만아니라 일반3d 엔티티를 선택할수도있는건데.
			//여기서 이벤트를 보내서 마우스피킹작업을 수행하도록한다던가.
			//근데 이 LButton Down메세지가 3d 게임시스템에도 전달이될거야
			//일단 ui에서 처리후에말이지
			//근데 여기서만약 선택되었다면 focus flag를 on해서 3d시스템에는 전달되지못하게할것이고
			//그렇지않으면 자연스럽게 전달되겠지 그럼 그곳에서 마우스피킹판정을 하지않을까?
			//그럼여기서는 해야하는것은 선택된다면 선택되지않는다면 focus flag를 켜거나 끌것


			//그리고 선택이 안된것은 확실하니 일단 기존에 선택되어진 엔티티에게 
			//ReleaseSelect 이벤트를 보낸다.

			MouseClickSelectEvent selectEvent("ReleaseSelect");
			mSelectedEntity->OnEvent(&selectEvent);

			//ui 엔티티내에서는 선택이안되었으니 default상태로 돌아간다.
			DefaultState* defaultState = new DefaultState("Default");
			defaultState->SetEventFocusFlag(false);
			//KeyBoard::SetTextBoxActiveFlag(false);
			return defaultState;

		}else if(object->GetSelectAvailableFlag())
		{

			//ui엔티티 마우스 피킹이되었다.
			if (object != mSelectedEntity)
			{
				//동일하지않으므로 기존의 선택엔티티에게 ReleaseSelect 이벤트를 보내고 
				MouseClickSelectEvent releaseSelectEvent("ReleaseSelect");
				mSelectedEntity->OnEvent(&releaseSelectEvent);
			}


			//새로 선택된 엔티티또는 이미선택했지만 다시 선택event전송 Select이벤트를보내고 
			MouseClickSelectEvent selectEvent("Select");
			selectEvent.SetRay(ray);
			object->OnEvent(&selectEvent);


			//마우스 클릭 이벤트를 등록된 모든 엔티티들에게 보낸다.
			std::vector<Object*> objectVector;
			eventTable->GetObjectVector("LButtonDown", objectVector);
			mouseEvent->SetRay(ray);

			for (int i = 0; i < objectVector.size(); ++i)
			{
				objectVector[i]->OnEvent(mouseEvent);
			}

			//마우스 클릭 상태로 전환한다.
			MouseClickState* mouseClickState = new MouseClickState("MouseClick");
			mouseClickState->SetEventFocusFlag(true);
			mouseClickState->SetSelectedEntity((UiEntity*)object);
			mouseClickState->SetMouseScreenPos(mouseEvent->GetClientPositionX(), mouseEvent->GetClientPositionY());
			return mouseClickState;
		}

		//ray판정이있겠다.

		DefaultState* defaultState = new DefaultState("Default");
		defaultState->SetEventFocusFlag(false);
		return defaultState;


		return nullptr;
	}

	SystemState* SystemSelectState::HandleMouseMove(System& system, Event* event)
	{
		//선택된 엔티티에 mouse move이벤트를보내
		//그리고 ray판정을해서 마우스가 위에존재하는 엔티티에게 hover이벤트를보내


		MouseEvent* mouseEvent = (MouseEvent*)event;

		Map* map = system.GetMap();
		//SpacePartitioningStructure* spacePartitioningStructure = map->GetSpaceParitioningStructure();
		CollisionWorld* collisionWorld = map->GetCollisionWorld();


		Object* object = nullptr;
		Ray ray;
		MathHelper::CalculateRay({ (float)mouseEvent->GetClientPositionX(), (float)mouseEvent->GetClientPositionY() }
		, system.GetViewPort(), *map->GetMainCamera(), true, ray);
		bool rayRet = collisionWorld->RayCastingFirst(object, ray);

		//선택된 엔티티에게 mousemove이벤트로보내고
		mouseEvent->SetRay(ray);
		mSelectedEntity->OnEvent(event);



		MouseEvent hoverEvent("Hover");
		hoverEvent.SetRay(ray);
		hoverEvent.SetClientPosition(mouseEvent->GetClientPositionX(), mouseEvent->GetClientPositionY());
		hoverEvent.SetScreenPosition(mouseEvent->GetScreenPositionX(), mouseEvent->GetScreenPositionY());
		hoverEvent.SetHoverState(true);

		if (rayRet)
		{
			//이전hover이벤트를 받은 엔티티와 다르다면 NotHover이벤트를 보낸다.
			if (mHoverObject != nullptr && mHoverObject != object)
			{
				hoverEvent.SetHoverState(false);
				mHoverObject->OnEvent(&hoverEvent);

			}
			hoverEvent.SetHoverState(true);
			object->OnEvent(&hoverEvent);
			mHoverObject = object;
		}
		else
		{
			//이전에 hoverevent를 받은 엔티티에게 NotHover이벤트를보낸다.
			if (mHoverObject != nullptr)
			{
				hoverEvent.SetHoverState(false);
				mHoverObject->OnEvent(&hoverEvent);
				mHoverObject = nullptr;
			}
		}

		return nullptr;
	}

	SystemState* SystemSelectState::HandleMouseInOut(System& system, Event* event)
	{
		MouseInOutEvent* mouseInOutEvent = (MouseInOutEvent*)event;

		switch (mouseInOutEvent->GetMouseState())
		{
		case EMouseInOut::eOut:
		{
			if (mHoverObject != nullptr)
			{
				MouseEvent hoverEvent("Hover");
				hoverEvent.SetHoverState(false);
				mHoverObject->OnEvent(&hoverEvent);
				mHoverObject = nullptr;
			}
		}
		return nullptr;
		}

	}
}