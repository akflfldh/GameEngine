#include "ResizeHandle.h"
#include"MouseEvent.h"
#include"Event/ResizeEvent.h"
#include"Event/EventTable.h"
#include"System.h"

void Quad::ResizeHandle::Initialize()
{
	UiEntity::Initialize();
	SetEffect("DefaultUi.effect");

}

Quad::ResizeHandle::ResizeHandle(const std::string& name)
	:UiEntity(name)
{
}

void Quad::ResizeHandle::Initialize(int direction)
{
	mDirection = direction;
	Initialize();
	System* system = GetSystem();	
	system->RegisterEntityToEvent("LButtonDown", this);


}

void Quad::ResizeHandle::Update(float deltaTime)
{



	UiEntity::Update(deltaTime);






}

void Quad::ResizeHandle::OnEvent(Event* event)
{

	StateComponent* stateComponent = GetStateComponent();

	std::string eventName = event->GetName();
	if (eventName == "MouseMove")
	{
		if (stateComponent->GetSelectState())	//select상태도아닌데 어떻게 움직였지
		{
			MouseEvent* mouseEvent = (MouseEvent*)event;
			
			INT distanceX = mouseEvent->GetClientPositionX() - mouseEvent->GetPreClientPositionX();
			INT distanceY = mouseEvent->GetClientPositionY() - mouseEvent->GetPreClientPositionY();
			
			DirectX::XMFLOAT3 mouseDir = { (float)distanceX ,-1.0f * distanceY, 0.0f };
			DirectX::XMFLOAT3 resizeHandleUpVector = GetTransform().GetUpWorld();
			DirectX::XMFLOAT3 dot;


			DirectX::XMStoreFloat3(&dot,
				DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&mouseDir), DirectX::XMLoadFloat3(&resizeHandleUpVector)));
			//switch (mDirection)
			//{
			//case 0: //left
			//{
			//	resizeHandleUpVector.x *= -1.0f;
			//}
			//case 1://right
			//{
			//	DirectX::XMStoreFloat3(&dot,
			//		DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&mouseDir), DirectX::XMLoadFloat3(&resizeHandleUpVector)));
			//}
			//break;

			//case 3://bottom
			//{
			//	resizeHandleUpVector.y *= -1.0f;
			//}
			//case 2://top
			//{
			//	DirectX::XMStoreFloat3(&dot,
			//		DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&mouseDir), DirectX::XMLoadFloat3(&resizeHandleUpVector)));
			//}
			//break;

			//}





			
			ResizeHandleEvent resizeEvent("Resize");
			resizeEvent.SetDirection(mDirection);
			float widthWorld = mDstEntity->GetTransform().GetWidthWorld();
			float heightWorld = mDstEntity->GetTransform().GetHeightWorld();
			float depthWorld = mDstEntity->GetTransform().GetDepthWorld();

			switch(mDirection)
			{
			case 0:		//left
			{
				widthWorld += dot.x;
			}
				break;		//right
			case 1:
			{
				widthWorld += dot.x;
			}
				break;		//top
			case 2:
				{
					heightWorld += dot.x;
				}
				break;
			case 3:			//bottom
			{
				heightWorld += dot.x;
			}
				break;
			}

			resizeEvent.SetNewSize({ widthWorld,heightWorld,depthWorld });
			mDstEntity->OnEvent(&resizeEvent);

		}

	}
	else
	{
		UiEntity::OnEvent(event);
	}
	SetDrawFlag(true);
	SetEntireDrawFlag(true);

	

}

void Quad::ResizeHandle::SetDstEntity(UiEntity* entity)
{
	mDstEntity = entity;
}
