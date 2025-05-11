#pragma once
#include"State.h"

namespace Quad
{

	enum class EClickableComponentState
	{
		eClickDown = 0,
		eClickUp,
		eClickAndMove,
		eMove
	};


	class ClickableComponentState :public State
	{
	public:
		ClickableComponentState(const std::string& name);
		~ClickableComponentState() = default;

		void EnterState() override;
		void RunState() override;
		void ExitState() override;

		//이거에대한 함수호출을 콜백이든 어떻게든 유저가 설정하는것으로 만들어야된다.

	private:


	};

}