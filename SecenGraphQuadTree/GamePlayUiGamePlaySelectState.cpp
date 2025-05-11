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

			//game play에서는 엔티티들에게 전적으로 그 event에대해서 어떻게 처리할지를 맏겨야한다
			//엔진에서 마우스가이동해서 그 오브젝트를 벗어났다고해서 클릭여부가 풀리거나 하는거는
			//여기서 판단하는게아니라 엔티티의 이벤트 핸들러가 결정하도록해야한다..

			//엔티티의 이벤트핸들러에서 범위에서 벗어나서  이제는 선택을 풀려면 그에 대한 이벤트를 보내면되겠다

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