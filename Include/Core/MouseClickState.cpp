#include "MouseClickState.h"
#include"MouseEvent.h"
#include"DefaultState.h"
#include"GamePlayUiSystem.h"
#include"GamePlayUiGamePlayState.h"
#include"SystemSelectState.h"
#include"GamePlayEvent.h"
#include"CollisionWorld.h"


#include"MathHelper.h"
#include"TextureManager.h"
#include"GamePlayWindowController.h"


#include"KeyBoard.h"
//#include"Text.h"

namespace Quad
{
	MouseClickState::MouseClickState(const std::string& name)
		:SystemState(name), mSelectedEntity(nullptr)
	{
	}

	void MouseClickState::EnterState(System& system)
	{
		int a = 2;
	}

	void MouseClickState::RunState(System& system)
	{
	}

	void MouseClickState::ExitState(System& system)
	{
	}

	SystemState* MouseClickState::HandleEvent(System& system, Event* event)
	{


		const std::string& eventName = event->GetName();
		MouseEvent* pMouseEvent = (MouseEvent*)event;
		if (eventName == "MouseMove")
		{


			Ray  ray;
			float screenPosX = (float)pMouseEvent->GetClientPositionX();
			float screenPosY = (float)pMouseEvent->GetClientPositionY();
			Map* map = system.GetMap();
			Camera* camera = map->GetMainCamera();
			MathHelper::CalculateRay({ screenPosX,screenPosY }, system.GetViewPort(), *camera, true, ray);
			CollisionWorld* collisionWorld = map->GetCollisionWorld();

			Object* object = nullptr;
			pMouseEvent->SetHoverState(false);
			if (collisionWorld->RayCastingFirst(object, ray))
			{
				if (object == mSelectedEntity)
				{
					//hover이다 . 즉 위에있다라는것에대한 플래그설정
					pMouseEvent->SetHoverState(true);
				}
			}
			mSelectedEntity->OnEvent(event);

			return nullptr;



		}
		else if (eventName == "LButtonUp")
		{

			//ray판정
			//여전히 select 엔티티 위인가?
			//위라면 LbuttonUp메세지에 특정플래그를설정해서보내자.
			//그렇지 않다면 설정 x 
			//그럼엔티티들에따라서 up할떄 자신위에있어야작동하게할수도있고 없어서 작동하게할수도있고.

			Ray  ray; 
			float screenPosX = (float)pMouseEvent->GetClientPositionX();
			float screenPosY = (float)pMouseEvent->GetClientPositionY();
			Map * map =system.GetMap();
			Camera* camera = map->GetMainCamera();
			MathHelper::CalculateRay({ screenPosX,screenPosY }, system.GetViewPort(), *camera, true, ray);
			CollisionWorld* collisionWorld = map->GetCollisionWorld();

			Object* object=nullptr;
			pMouseEvent->SetHoverState(false);
			if (collisionWorld->RayCastingFirst(object, ray))
			{
				if (object == mSelectedEntity)
				{
					pMouseEvent->SetHoverState(true);
				}
			}
			//up event;
			mSelectedEntity->OnEvent(event);


			//선택이 유지되는상태. object의 selectAvailableflag에따라 결정된다.

			if (mSelectedEntity->GetSelectKeepingFlag())
			{
				SystemSelectState* selectState = new SystemSelectState("Select");
				selectState->SetEventFocusFlag(true);
				selectState->SetSelectedEntity(mSelectedEntity);
				return selectState;
			}
			else
			{
				DefaultState* defaultState = new DefaultState("Default");
				return defaultState;
			}

		}

		return nullptr;

	}

	void MouseClickState::SetSelectedEntity(UiEntity* entity)
	{
		mSelectedEntity = entity;
	}

	UiEntity* MouseClickState::GetSelectedEntity() const
	{
		return mSelectedEntity;
	}

	void MouseClickState::SetMouseScreenPos(UINT x, UINT y)
	{
		mMouseScreenPosX = x;
		mMouseScreenPosY = y;
	}

	UINT MouseClickState::GetMouseScreenPosX() const
	{
		return mMouseScreenPosX;
	}

	UINT MouseClickState::GetMouseScreenPosY() const
	{
		return mMouseScreenPosY;
	}


}