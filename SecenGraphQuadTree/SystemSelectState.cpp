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
		//Ű�� �����µ� ������� �׷��� �� ��ƼƼ������ ����Ұ��ΰ� �ᱹ�װ� ���� �ڵ� ���̺���
		//����ϴ°��� �� �ٶ����Ҽ��ִ�.
		//�����鿡��

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
		//���� �ٸ����� �����ٸ� 
			//����Ʈ���¶Ǵ� �ٸ� ��ƼƼ�� ��������(click state�� ����)

			//�ٽ� ray������ ����
			//�׸���� �ٽ� mouse click ���� �̵�  (���οƼƼ�̰� ���������� ��ƼƼ�̰� Ŭ�����·� ����)
			//���� �������ð��ִ� ��ƼƼ�ϼ����ְ� �ƴҼ����ְ� �ƴѰ�� ReleaseSelect�̺�Ʈ�� �����ش�.
			//���οƼƼ�ΰ�� Select�̺�Ʈ�� �����ش�.

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
			// �Ǵ��� �������� ������ ui�Ӹ��ƴ϶� �Ϲ�3d ��ƼƼ�� �����Ҽ����ִ°ǵ�.
			//���⼭ �̺�Ʈ�� ������ ���콺��ŷ�۾��� �����ϵ����Ѵٴ���.
			//�ٵ� �� LButton Down�޼����� 3d ���ӽý��ۿ��� �����̵ɰž�
			//�ϴ� ui���� ó���Ŀ�������
			//�ٵ� ���⼭���� ���õǾ��ٸ� focus flag�� on�ؼ� 3d�ý��ۿ��� ���޵������ϰ��Ұ��̰�
			//�׷��������� �ڿ������� ���޵ǰ��� �׷� �װ����� ���콺��ŷ������ ����������?
			//�׷����⼭�� �ؾ��ϴ°��� ���õȴٸ� ���õ����ʴ´ٸ� focus flag�� �Ѱų� ����


			//�׸��� ������ �ȵȰ��� Ȯ���ϴ� �ϴ� ������ ���õǾ��� ��ƼƼ���� 
			//ReleaseSelect �̺�Ʈ�� ������.

			MouseClickSelectEvent selectEvent("ReleaseSelect");
			mSelectedEntity->OnEvent(&selectEvent);

			//ui ��ƼƼ�������� �����̾ȵǾ����� default���·� ���ư���.
			DefaultState* defaultState = new DefaultState("Default");
			defaultState->SetEventFocusFlag(false);
			//KeyBoard::SetTextBoxActiveFlag(false);
			return defaultState;

		}else if(object->GetSelectAvailableFlag())
		{

			//ui��ƼƼ ���콺 ��ŷ�̵Ǿ���.
			if (object != mSelectedEntity)
			{
				//�������������Ƿ� ������ ���ÿ�ƼƼ���� ReleaseSelect �̺�Ʈ�� ������ 
				MouseClickSelectEvent releaseSelectEvent("ReleaseSelect");
				mSelectedEntity->OnEvent(&releaseSelectEvent);
			}


			//���� ���õ� ��ƼƼ�Ǵ� �̹̼��������� �ٽ� ����event���� Select�̺�Ʈ�������� 
			MouseClickSelectEvent selectEvent("Select");
			selectEvent.SetRay(ray);
			object->OnEvent(&selectEvent);


			//���콺 Ŭ�� �̺�Ʈ�� ��ϵ� ��� ��ƼƼ�鿡�� ������.
			std::vector<Object*> objectVector;
			eventTable->GetObjectVector("LButtonDown", objectVector);
			mouseEvent->SetRay(ray);

			for (int i = 0; i < objectVector.size(); ++i)
			{
				objectVector[i]->OnEvent(mouseEvent);
			}

			//���콺 Ŭ�� ���·� ��ȯ�Ѵ�.
			MouseClickState* mouseClickState = new MouseClickState("MouseClick");
			mouseClickState->SetEventFocusFlag(true);
			mouseClickState->SetSelectedEntity((UiEntity*)object);
			mouseClickState->SetMouseScreenPos(mouseEvent->GetClientPositionX(), mouseEvent->GetClientPositionY());
			return mouseClickState;
		}

		//ray�������ְڴ�.

		DefaultState* defaultState = new DefaultState("Default");
		defaultState->SetEventFocusFlag(false);
		return defaultState;


		return nullptr;
	}

	SystemState* SystemSelectState::HandleMouseMove(System& system, Event* event)
	{
		//���õ� ��ƼƼ�� mouse move�̺�Ʈ������
		//�׸��� ray�������ؼ� ���콺�� ���������ϴ� ��ƼƼ���� hover�̺�Ʈ������


		MouseEvent* mouseEvent = (MouseEvent*)event;

		Map* map = system.GetMap();
		//SpacePartitioningStructure* spacePartitioningStructure = map->GetSpaceParitioningStructure();
		CollisionWorld* collisionWorld = map->GetCollisionWorld();


		Object* object = nullptr;
		Ray ray;
		MathHelper::CalculateRay({ (float)mouseEvent->GetClientPositionX(), (float)mouseEvent->GetClientPositionY() }
		, system.GetViewPort(), *map->GetMainCamera(), true, ray);
		bool rayRet = collisionWorld->RayCastingFirst(object, ray);

		//���õ� ��ƼƼ���� mousemove�̺�Ʈ�κ�����
		mouseEvent->SetRay(ray);
		mSelectedEntity->OnEvent(event);



		MouseEvent hoverEvent("Hover");
		hoverEvent.SetRay(ray);
		hoverEvent.SetClientPosition(mouseEvent->GetClientPositionX(), mouseEvent->GetClientPositionY());
		hoverEvent.SetScreenPosition(mouseEvent->GetScreenPositionX(), mouseEvent->GetScreenPositionY());
		hoverEvent.SetHoverState(true);

		if (rayRet)
		{
			//����hover�̺�Ʈ�� ���� ��ƼƼ�� �ٸ��ٸ� NotHover�̺�Ʈ�� ������.
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
			//������ hoverevent�� ���� ��ƼƼ���� NotHover�̺�Ʈ��������.
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