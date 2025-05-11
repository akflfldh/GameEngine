#include "LookComponent.h"
#include"Object/Object.h"

#include"Core/InputSystem.h"




Quad::LookComponent::LookComponent(Object * destObject)
	:mDestObject(destObject),mDeltaX(0), mDeltaY(0), mRotationScreenX(0), mRotationScreenY(0)
{
	auto inputSystem = InputSystem::GetInstance();


	inputSystem->RegisterAxisBehaviorKeyValue(kMouseMoveX, 1.0f, "MouseMoveX");
	inputSystem->RegisterAxisBehaviorKeyValue(kMouseMoveY, 1.0f, "MouseMoveY");

	inputSystem->RegisterBehaviorCallbackAxis<LookComponent, &LookComponent::TurnPitchInput>("MouseMoveY", this);
	inputSystem->RegisterBehaviorCallbackAxis<LookComponent, &LookComponent::TurnYawInput>("MouseMoveX", this);

}

Quad::LookComponent::~LookComponent()
{
	auto inputSystem = InputSystem::GetInstance();

	inputSystem->ReleaseBehaviorCallbackAxis<LookComponent, &LookComponent::TurnPitchInput>("MouseMoveY", this);
	inputSystem->ReleaseBehaviorCallbackAxis<LookComponent, &LookComponent::TurnYawInput>("MouseMoveX", this);


}

void Quad::LookComponent::Update(float delta)
{
	Turn(delta);

}

void Quad::LookComponent::TurnPitchInput(float value)
{

	mDeltaY = value;
}

void Quad::LookComponent::TurnYawInput(float value)
{
	mDeltaX = value;

}

void Quad::LookComponent::TurnPitch(float deltaTime)
{




}

void Quad::LookComponent::TurnYaw(float deltaTime)
{




}

void Quad::LookComponent::Turn(float deltaTime)
{

	
	
	if (mDeltaX == 0.0f && mDeltaY == 0.0f)
		return;


	Transform & transform =	mDestObject->GetTransform();

	DirectX::XMFLOAT4 currQuaternionWF = transform.GetQuaternionWorld();
	DirectX::XMVECTOR currQuaternionW = DirectX::XMLoadFloat4(&currQuaternionWF);
	

	//DirectX::XMFLOAT3 upVector = transform.GetUpWorld();
	//DirectX::XMFLOAT3 rightVector = transform.GetRightWorld();

	if (mDeltaX != 0)
	{
		DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0, 1, 0, 0);

		float radX = mDeltaX * DirectX::XM_PI / 180.0f ;

		DirectX::XMVECTOR addXQuaternion =	DirectX::XMQuaternionRotationAxis(upVector, radX);
		
		currQuaternionW = DirectX::XMQuaternionMultiply(currQuaternionW, addXQuaternion);
		

	}

	if (mDeltaY != 0)
	{
		//DirectX::XMFLOAT3 rightVector = transform.GetRightWorld();
		DirectX::XMVECTOR rightVector = DirectX::XMVectorSet(1.0f, 0, 0, 0);
		
		rightVector = DirectX::XMVector3Rotate(rightVector, currQuaternionW);

		float radY = mDeltaY* DirectX::XM_PI / 180.0f ;

		//DirectX::XMVECTOR addYQuaternion = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&rightVector), radY);
		DirectX::XMVECTOR addYQuaternion = DirectX::XMQuaternionRotationAxis(rightVector, radY);

		currQuaternionW = DirectX::XMQuaternionMultiply(currQuaternionW, addYQuaternion);
		
	}



	DirectX::XMStoreFloat4(&currQuaternionWF, DirectX::XMQuaternionNormalize(currQuaternionW));
	transform.SetQuaternionWorld(currQuaternionWF);




	mDeltaX = mDeltaY = 0.0f;

}
