#pragma once


#include"SystemState.h"

#include"Object/UiEntity.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB MouseClickState :public SystemState
	{
	public:
		MouseClickState(const std::string& name);
		~MouseClickState() = default;



		virtual void EnterState(System& system)override;
		virtual void RunState(System& system)override;			//run state은 현재 상태에서 매프레임마다 호출될때
		virtual void ExitState(System& system)override;


		virtual SystemState* HandleEvent(System& system, Event* event) override;		//현재상태에서 이벤트가 발생하여 들어올때 호출

		//고유한 메서드를 둔다
		void SetSelectedEntity(UiEntity* entity);
		UiEntity* GetSelectedEntity() const;

		void SetMouseScreenPos(UINT x, UINT y);
		UINT GetMouseScreenPosX() const;
		UINT GetMouseScreenPosY() const;

		//또는 variant를 인수로 받는 가상함수를 만들어서 처리하자 (EnterState,등등에서)


	private:
		UiEntity* mSelectedEntity;
		UINT mMouseScreenPosX;
		UINT mMouseScreenPosY;


	};

}

