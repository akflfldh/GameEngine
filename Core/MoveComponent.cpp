#include "MoveComponent.h"
#include"Object/Object.h"
#include"Core/InputSystem.h"

Quad::MoveComponent::MoveComponent(Object* destObject)
	:mDestObject(destObject), mSpeed(1.0f),mForwardDir(0.0f), mSideDir(0.0f)
{
	auto inputSystem = InputSystem::GetInstance();
	inputSystem->RegisterAxisBehaviorKeyValue('W', 1.0f, "MoveFoward");
	inputSystem->RegisterAxisBehaviorKeyValue('S', -1.0f, "MoveFoward");
	inputSystem->RegisterAxisBehaviorKeyValue('A', -1.0f, "MoveSide");
	inputSystem->RegisterAxisBehaviorKeyValue('D', +1.0f, "MoveSide");
	inputSystem->RegisterBehaviorCallbackAxis<MoveComponent, &MoveComponent::MoveForwardInput>("MoveFoward", this);
	inputSystem->RegisterBehaviorCallbackAxis<MoveComponent, &MoveComponent::MoveSideInput>("MoveSide", this);

}

Quad::MoveComponent::~MoveComponent()
{

	auto inputSystem = InputSystem::GetInstance();

	inputSystem->ReleaseBehaviorCallbackAxis<MoveComponent, &MoveComponent::MoveForwardInput>("MoveFoward", this);
	inputSystem->ReleaseBehaviorCallbackAxis<MoveComponent, &MoveComponent::MoveSideInput>("MoveSide", this);


}

void Quad::MoveComponent::Update(float deltaTime)
{
	//MoveForward(deltaTime);
	//MoveSide(deltaTime);
	Move(deltaTime);

}

void Quad::MoveComponent::SetSpeed(float speed)
{
	mSpeed = speed;

}

void Quad::MoveComponent::MoveForwardInput(float axis)
{

	mForwardDir = axis;
	
	 

	if (axis == 1.0f)
	{

	}
	else if (axis == -1.0f)
	{



	}






}

void Quad::MoveComponent::MoveSideInput(float axis)
{

	mSideDir = axis;



}

void Quad::MoveComponent::MoveForward(float deltaTime)
{
	if (mForwardDir == 0.0f)
		return;


	Transform & transform =	mDestObject->GetTransform();

	DirectX::XMFLOAT3 lookWorld = transform.GetLookWorld();
	DirectX::XMFLOAT3 posWorld = transform.GetPositionWorld();

	DirectX::XMStoreFloat3(&posWorld,DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&posWorld),DirectX::XMVectorScale(DirectX::XMLoadFloat3(&lookWorld), mForwardDir*mSpeed * deltaTime)));


	transform.SetPositionWorld(posWorld);

	mForwardDir = 0.0f;
}

void Quad::MoveComponent::MoveSide(float deltaTime)
{


	if (mSideDir == 0.0f)
		return;
	Transform& transform = mDestObject->GetTransform();
	DirectX::XMFLOAT3 rightWorld = transform.GetRightWorld();
	DirectX::XMFLOAT3 posWorld = transform.GetPositionWorld();

	DirectX::XMStoreFloat3(&posWorld, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&posWorld), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&rightWorld), mSideDir * mSpeed * deltaTime)));

	transform.SetPositionWorld(posWorld);

	mSideDir = 0.0f;











}

void Quad::MoveComponent::Move(float deltaTime)
{
	if (mForwardDir == 0.0f && mSideDir == 0.0f)
		return;
	Transform& transform = mDestObject->GetTransform();

	DirectX::XMFLOAT3 posWorld = transform.GetPositionWorld();
	if (mForwardDir != 0.0f)
	{
		DirectX::XMFLOAT3 lookWorld = transform.GetLookWorld();
		DirectX::XMStoreFloat3(&posWorld, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&posWorld), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&lookWorld), mForwardDir * mSpeed * deltaTime)));

		mForwardDir = 0.0f;
	}

	if (mSideDir != 0.0f)
	{

		DirectX::XMFLOAT3 rightWorld = transform.GetRightWorld();
		DirectX::XMStoreFloat3(&posWorld, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&posWorld), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&rightWorld), mSideDir * mSpeed * deltaTime)));
		mSideDir = 0.0f;
	}
	std::wstring output = std::to_wstring(posWorld.z);
	OutputDebugString(output.c_str());
	OutputDebugString(L"\n");


	transform.SetPositionWorld(posWorld);




}
