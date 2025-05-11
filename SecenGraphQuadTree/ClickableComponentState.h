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

		//�̰ſ����� �Լ�ȣ���� �ݹ��̵� ��Ե� ������ �����ϴ°����� �����ߵȴ�.

	private:


	};

}