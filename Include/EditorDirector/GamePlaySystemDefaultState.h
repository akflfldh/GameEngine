#pragma once
#include"Predeclare.h"


#include "DefaultState.h"


namespace Quad
{
    class GamePlaySystemDefaultState :public DefaultState
    {
    public:
		GamePlaySystemDefaultState(const std::string& name);
		~GamePlaySystemDefaultState() = default;



		virtual SystemState* HandleEvent(System & system, Event * event) override;		//이벤트를 처리하고나서
        
	private:

        void HandleNotifyDragAndDropEvent(System& system,Event* event);
        SystemState* HandleLButtonDownEvent(System& system, Event* event) override;


    private:
        void HandleNotifyDragAndDropAsset(System& system,Asset * asset, int screenPosX, int screenPosY, bool lButtonUpState);


    private:
        //mesh시를 드래그 할때  여기서 가지는 엔티티가 새로운 준비된 엔티티인지여부
        bool mNewEntityFlag = true;
        bool mEntityInSystemFlag = false; //엔티티가 이미 시스템안에 추가되었는가여부
        
        

        bool mMouseEnterSceneFlag = false;//마우스가 화면안으로 들어올떄 그떄를나타내는플래그
        bool mEnterSceneFirstFlag = true;

        Entity* mEntity;

    };

}
