#include "GamePlayWindowCamera.h"
#include"Event/EventTable.h"
#include"System.h"
#include"Mouse.h"
#include"KeyEvent.h"
#include"MouseEvent.h"
Quad::GamePlayWindowCamera::GamePlayWindowCamera(const std::string& name)
	:FrustumCamera(),mASpeed(0),mSSpeed(0),mDSpeed(0),mWSpeed(0)
{
	SetEngineObjectFlag(true);

	
}

void Quad::GamePlayWindowCamera::Initialize()
{
	FrustumCamera::Initialize();
	EventTable* eventTable = GetSystem()->GetEventTable();
	eventTable->AddObject("RButtonDown", this);
	eventTable->AddObject("RButtonUp", this);
	eventTable->AddObject("MouseMove", this);
	eventTable->AddObject("KeyDown", this);
	eventTable->AddObject("KeyUp", this);


	mKeyADownState = false;
	mKeyWDownState = false;
	mKeyDDownState = false;
	mKeySDownState = false;
	mRotationScreenX = 0;
	mRotationScreenY = 0;



}

Quad::GamePlayWindowCamera* Quad::GamePlayWindowCamera::Create(Map* map, int mapLayerIndex, float fovY, float aspect)
{
	GamePlayWindowCamera* ob = static_cast<GamePlayWindowCamera*>(map->CreateObject("GamePlayWindowCamera", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating(fovY, aspect);



	return ob;
}




void Quad::GamePlayWindowCamera::Update(float deltaTime)
{
	Camera::Update(deltaTime);

	if (mKeyADownState)
	{
		if (mASpeed < 1.0f)
			mASpeed += 0.01f;

	

		const DirectX::XMFLOAT3& rightDir = GetTransform().GetRightWorld();

		DirectX::XMVECTOR shift = DirectX::XMVectorMultiply(DirectX::XMVectorSet(mASpeed, mASpeed, mASpeed, 0.0f), DirectX::XMLoadFloat3(&rightDir));
		DirectX::XMFLOAT3 pos = GetTransform().GetPositionWorld();

		DirectX::XMStoreFloat3(&pos, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), shift));

		GetTransform().SetPositionWorld(pos);
	}

	if (mKeyDDownState)
	{
		if (mDSpeed < 1.0f)
		mDSpeed += 0.01f;

		const DirectX::XMFLOAT3& rightDir = GetTransform().GetRightWorld();

		DirectX::XMVECTOR shift = DirectX::XMVectorMultiply(DirectX::XMVectorSet(mDSpeed, mDSpeed, mDSpeed, 0.0f), DirectX::XMLoadFloat3(&rightDir));
		DirectX::XMFLOAT3 pos = GetTransform().GetPositionWorld();

		DirectX::XMStoreFloat3(&pos, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&pos), shift));

		GetTransform().SetPositionWorld(pos);
	}

	if (mKeyWDownState)
	{
		if (mWSpeed < 1.0f)
			mWSpeed += 0.01f;

		const DirectX::XMFLOAT3& lookDir = GetTransform().GetLookWorld();

		DirectX::XMVECTOR shift = DirectX::XMVectorMultiply(DirectX::XMVectorSet(mWSpeed, mWSpeed, mWSpeed, 0.0f), DirectX::XMLoadFloat3(&lookDir));


		DirectX::XMFLOAT3 pos = GetTransform().GetPositionWorld();

		DirectX::XMStoreFloat3(&pos, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&pos), shift));

		GetTransform().SetPositionWorld(pos);
	}

	if (mKeySDownState)
	{
		if (mSSpeed < 1.0f)
		mSSpeed += 0.01f;

		const DirectX::XMFLOAT3& lookDir = GetTransform().GetLookWorld();
		DirectX::XMVECTOR shift = DirectX::XMVectorMultiply(DirectX::XMVectorSet(mSSpeed, mSSpeed, mSSpeed, 0.0f), DirectX::XMLoadFloat3(&lookDir));


		DirectX::XMFLOAT3 pos = GetTransform().GetPositionWorld();

		

		DirectX::XMStoreFloat3(&pos, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pos), shift));

		GetTransform().SetPositionWorld(pos);
	}



}



void Quad::GamePlayWindowCamera::OnEvent(Event* event)
{
	Camera::OnEvent(event);

	const std::string& eventName = event->GetName();

	if (eventName == "RButtonDown")
	{
		HandleRButtonDown(event);
	}
	else if (eventName == "RButtonUp")
	{
		HandleRButtonUp(event);
	}
	else if (eventName == "MouseMove")
	{
		HandleMouseMove(event);
	}
	else if (eventName == "KeyDown")
	{
		HandleKeyDown(event);
	}
	else if (eventName == "KeyUp")
	{
		HandleKeyUp(event);
	}

	



}

void Quad::GamePlayWindowCamera::InitCreating(float fovY, float aspect)
{
	FrustumCamera::InitCreating(fovY, aspect);


}

void Quad::GamePlayWindowCamera::HandleRButtonDown(Event* pEvent)
{
	//mouse move
	//rbutton up
	//key down이벤트를 받도록 설정한다.
	
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;

	mRButtonDownClientPosX= 	mouseEvent->GetClientPositionX();
	mRButtonDownClientPosY= 	mouseEvent->GetClientPositionY();
	mRButtonDownScreenPosX= 	mouseEvent->GetScreenPositionX();
	mRButtonDownScreenPosY= 	mouseEvent->GetScreenPositionY();

	while(ShowCursor(false)>=0);



}

void Quad::GamePlayWindowCamera::HandleRButtonUp(Event* pEvent)
{

	SetCursorPos(mRButtonDownScreenPosX, mRButtonDownScreenPosY);


	while (ShowCursor(true) < 0);


}

void Quad::GamePlayWindowCamera::HandleMouseMove(Event* pEvent)
{


	if (Mouse::GetRButtonState())
	{

		MouseEvent* mouseEvent = (MouseEvent*)pEvent;

		float shiftScreenX = mouseEvent->GetClientPositionX() - mouseEvent->GetPreClientPositionX();
		float shiftScreenY = mouseEvent->GetClientPositionY() - mouseEvent->GetPreClientPositionY();
		mRotationScreenX += shiftScreenX;
		mRotationScreenY += shiftScreenY;
		if (mRotationScreenY >= 90.0f)
			mRotationScreenY = 90.0f;
		else if (mRotationScreenY <= -90.0f)
			mRotationScreenY = -90.0f;

		GetTransform().SetRotationLocal({ mRotationScreenY,mRotationScreenX ,0 });

	}

}

void Quad::GamePlayWindowCamera::HandleKeyDown(Event* pEvent)
{
	

	if (Mouse::GetRButtonState())
	{
		KeyEvent* keyEvent = (KeyEvent*)pEvent;
		int key =keyEvent->GetKey();
		if (key == 'A')
		{
			mKeyADownState = true;
		}
		else if (key == 'D')
		{
			mKeyDDownState = true;


		}
		else if (key == 'W')
		{
			
			mKeyWDownState = true;

		}
		else if (key == 'S')
		{
			mKeySDownState = true;
		}


	}

}

void Quad::GamePlayWindowCamera::HandleKeyUp(Event* pEvent)
{

	KeyEvent* keyEvnet = (KeyEvent*)pEvent;

	int key = keyEvnet->GetKey();
	if (key == 'A')
	{
		mKeyADownState = false;
		mASpeed = 0;
	}
	else if (key == 'D')
	{
		mKeyDDownState = false;
		mDSpeed = 0;
	}
	else if (key == 'W')
	{
		mKeyWDownState = false;
		mWSpeed = 0;
	}
	else if (key == 'S')
	{
		mKeySDownState = false;
		mSSpeed = 0;
	}


}
