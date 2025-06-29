#include "FrustumColliderComponent.h"

#include"Collision/FrustumCollider.h"

Quad::FrustumColliderComponent::FrustumColliderComponent()
{
}

Quad::FrustumColliderComponent::~FrustumColliderComponent()
{
}

void Quad::FrustumColliderComponent::Start()
{
}

void Quad::FrustumColliderComponent::Update(float delta)
{
}

void Quad::FrustumColliderComponent::SetNearPlaneLocal(float nearPlane)
{
	mFrustumCollider->SetNearPlaneLocal(nearPlane);

}

void Quad::FrustumColliderComponent::SetFarPlaneLocal(float farPlane)
{
	mFrustumCollider->SetFarPlaneLocal(farPlane);
}

void Quad::FrustumColliderComponent::SetRightSlopeLocal(float rightSlope)
{
	mFrustumCollider->SetRightSlopeLocal(rightSlope);
}

void Quad::FrustumColliderComponent::SetLeftSlopeLocal(float leftSlope)
{
	mFrustumCollider->SetLeftSlopeLocal(leftSlope);
}

void Quad::FrustumColliderComponent::SetTopSlopeLocal(float topSlope)
{
	mFrustumCollider->SetTopSlopeLocal(topSlope);

}

void Quad::FrustumColliderComponent::SetBottomSlopeLocal(float bottomSlope)
{
	mFrustumCollider->SetBottomSlopeLocal(bottomSlope);
}

void Quad::FrustumColliderComponent::SetPositionLocal(const DirectX::XMFLOAT3& origin)
{
	mFrustumCollider->SetPositionLocal(origin);
}

void Quad::FrustumColliderComponent::SetOrientationLocal(const DirectX::XMFLOAT4& orientation)
{
	mFrustumCollider->SetOrientationLocal(orientation);
}

float Quad::FrustumColliderComponent::GetNearPlaneLocal() const
{
	return mFrustumCollider->GetNearPlaneLocal();
}

float Quad::FrustumColliderComponent::GetFarPlaneLocal() const
{
	return mFrustumCollider->GetFarPlaneLocal();
}

float Quad::FrustumColliderComponent::GetRightSlopeLocal() const
{
	return mFrustumCollider->GetRightSlopeLocal();
}

float Quad::FrustumColliderComponent::GetLeftSlopeLocal() const
{
	return mFrustumCollider->GetLeftSlopeLocal();
}

float Quad::FrustumColliderComponent::GetTopSlopeLocal() const
{
	return mFrustumCollider->GetTopSlopeLocal();
}

float Quad::FrustumColliderComponent::GetBottomSlopeLocal() const
{
	return mFrustumCollider->GetBottomSlopeLocal();
}

const DirectX::XMFLOAT3 Quad::FrustumColliderComponent::GetPositionLocal() const
{
	return mFrustumCollider->GetPositionLocal();
}

const DirectX::XMFLOAT4 Quad::FrustumColliderComponent::GetOrientationLocal() const
{
	return mFrustumCollider->GetOrientationLocal();
}

void Quad::FrustumColliderComponent::SetConfigLocal(float nearPlane, float farPlane, float rightSlope, float leftSlope, float topSlope, float bottomSlope, const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT4& orientation)
{
	mFrustumCollider->SetConfigLocal(nearPlane, farPlane, rightSlope, leftSlope, topSlope, bottomSlope, origin, orientation);
}

Quad::FrustumCollider* Quad::FrustumColliderComponent::GetCollider() const
{
	const Transform& transform = GetTransform();
	
	if(transform.GetDirtyFlag())
		mFrustumCollider->Update(GetTransformWorldMatrixF());


	return mFrustumCollider;
}
