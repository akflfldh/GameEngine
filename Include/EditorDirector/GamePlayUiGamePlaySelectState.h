#pragma once

#include"SystemState.h"

class Object;
namespace Quad
{
	class GamePlayUiGamePlaySelectState :public SystemState
	{
	public:
		GamePlayUiGamePlaySelectState(const std::string& name);
		~GamePlayUiGamePlaySelectState() = default;


		virtual void EnterState(System& system) override;
		virtual void RunState(System& system) override;
		virtual void ExitState(System& syetem) override;

		virtual SystemState* HandleEvent(System& syetem, Event* event) override;

		void SetSelectObject(Object* object);
		Object* GetSelectObject() const;
	private:



	private:
		Object* mSelectObject;		//포인터를 썻을때 오브젝트가 제거된다면 어떻게할것인가 .이런문제들이있겠죠
		//핸들이 좋은 방안이될수있을거같다.

	};

}