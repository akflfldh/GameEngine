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


		////�׸��� ���õ� �̺�Ʈ�� ���õ� ������Ʈ���Ժ�����
		////(����� RAY�� �ʿ��ϰ�)

		//MouseEvent mouseSelectEvent("MouseSelect");
		//mouseSelectEvent.SetClientPosition(pMouseEvent->GetClientPositionX(), pMouseEvent->GetClientPositionY());

		//Ray ray;
		//MathHelper::CalculateRay({ (float)pMouseEvent->GetClientPositionX(),(float)pMouseEvent->GetClientPositionY() },
		//	system.GetViewPort(), *system.GetMap()->GetGameCamera(), true, ray);


		////���⼭ MouseClickSelectEvent�� �����ؼ� �� �̺�Ʈ�� ���õɼ��ִ� ������Ʈ�鿡�� ������.
		////�׸��� ���¸� SelecteState�� �ٲ۴�
		////���� ���õȰԾ��ٸ� 
		////���콺 Ŭ���̺�Ʈ�� ���콺 Ŭ���̺�Ʈ�� �������մ� ������Ʈ�鿡�� ��� ������.

		////�ݸ������带 ���ұ� �ƴϸ� �����ұ� , �ݸ������带 ���ؼ��ϵ� �÷��׸������س��� �ϸ�Ǳ⵵�ϰ� 

		//
		//
		////�浹�������ϰ� �浹�� ��ƼƼ�������� �׸��� �� ��ƼƼ�� ���⼭ ���õɼ��ִ��÷��װ��ִ���Ȯ���ϴ°���










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

		//	//���¸� Object�� �����ѻ��·� ��ȯ

		//	GamePlayUiGamePlaySelectState* selectState = new GamePlayUiGamePlaySelectState("GamePlayUiGameSelectState");
		//	selectState->SetSelectObject(selectObjectVector[minIndex]);

		//	//�����Ѱ��������� �����ϴϱ� ���ŵǴ� ������Ʈ�ϼ������� �׷��ϱ� �����ʹ� �����ϴ�.

		//	selectState->SetEventFocusFlag(true);
		//	//false�̳� �̰Ŵ� ���

		//	return selectState;
		//}






		////Ŭ���ϴ� �̺�Ʈ�� ��ο��� 
		////(����� �׳� �������)

		//std::vector<Object*> objectVector;
		//eventTable->GetObjectVector("LButtonDown", objectVector);

		//for (int i = 0; i < objectVector.size(); ++i)
		//{
		//	objectVector[i]->OnEvent(event);
		//}

		////�̺�Ʈ�� �� �����°��� ��ȿ��������
		////�̺�Ʈ���� �� �̺�Ʈ�� ���� ������Ʈ�� ����ؼ� ��������.
		////�������� �� �̺�Ʈ�� ����ϰԸ���°��� (���� �����÷��̽ÿ���ȿ�Ѱ��� )
		////(�����÷������̾ƴҶ��� �������� �����ϴ°Ŵ��)



		//return nullptr;


		return nullptr;


	}

}