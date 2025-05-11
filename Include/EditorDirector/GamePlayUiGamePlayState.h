#pragma once

#include"SystemState.h"
namespace Quad
{
	class GamePlayUiGamePlayState :public SystemState
	{
	public:
		GamePlayUiGamePlayState(const std::string& name);
		~GamePlayUiGamePlayState() = default;


		virtual void EnterState(System& system) override;
		virtual void RunState(System& system) override;
		virtual void ExitState(System& syetem) override;

		virtual SystemState* HandleEvent(System& system, Event* event) override;

	private:
		SystemState* LButtonDown(System& system, Event* event);


	private:



	};

}
