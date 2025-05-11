#include "GamePlayUiGamePlaySelectState.h"
#include"GamePlayUiGamePlayState.h"
#include"Object.h"

namespace Quad
{

	GamePlayUiGamePlaySelectState::GamePlayUiGamePlaySelectState(const std::string& name)
		:SystemState(name)
	{
	}

	void GamePlayUiGamePlaySelectState::EnterState(System& system)
	{
	}

	void GamePlayUiGamePlaySelectState::RunState(System& system)
	{
	}

	void GamePlayUiGamePlaySelectState::ExitState(System& syetem)
	{
	}

	SystemState* GamePlayUiGamePlaySelectState::HandleEvent(System& syetem, Event* event)
	{
		const std::string& eventName = event->GetName();

		if (eventName == "MouseMove")
		{

			//game play������ ��ƼƼ�鿡�� �������� �� event�����ؼ� ��� ó�������� ���ܾ��Ѵ�
			//�������� ���콺���̵��ؼ� �� ������Ʈ�� ����ٰ��ؼ� Ŭ�����ΰ� Ǯ���ų� �ϴ°Ŵ�
			//���⼭ �Ǵ��ϴ°Ծƴ϶� ��ƼƼ�� �̺�Ʈ �ڵ鷯�� �����ϵ����ؾ��Ѵ�..

			//��ƼƼ�� �̺�Ʈ�ڵ鷯���� �������� �����  ������ ������ Ǯ���� �׿� ���� �̺�Ʈ�� ������ǰڴ�

			mSelectObject->OnEvent(event);
		}
		else if (eventName == "LButtonUp")
		{

			mSelectObject->OnEvent(event);
			GamePlayUiGamePlayState* gamePlayDefaultState = new GamePlayUiGamePlayState("Default");

			return gamePlayDefaultState;


		}
		return nullptr;

	}

	void GamePlayUiGamePlaySelectState::SetSelectObject(Object* object)
	{
		mSelectObject = object;
	}

	Object* GamePlayUiGamePlaySelectState::GetSelectObject() const
	{
		return mSelectObject;
	}


}