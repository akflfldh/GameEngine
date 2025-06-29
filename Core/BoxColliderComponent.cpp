#include "BoxColliderComponent.h"
#include"Collision/BoxCollider.h"
#include"Collision/ColliderGenerator.h"

Quad::BoxColliderComponent::BoxColliderComponent()
{
	mBoxCollider =static_cast<BoxCollider*>(ColliderGenerator::CreateBoxCollider());
}

Quad::BoxColliderComponent::~BoxColliderComponent()
{
	ColliderGenerator::ReleaseCollider(mBoxCollider);
}

void Quad::BoxColliderComponent::Initialize()
{
}

void Quad::BoxColliderComponent::Start()
{
}

void Quad::BoxColliderComponent::Update(float delta)
{


}

void Quad::BoxColliderComponent::SetMinMaxVertexLocal(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max)
{

	mBoxCollider->SetMinMaxVertexLocal(min, max);
}



void Quad::BoxColliderComponent::SetWidthLocalX(float width)
{
	mBoxCollider->SetWidthLocalX(width);
}

void Quad::BoxColliderComponent::SetHeightLocalY(float height)
{
	mBoxCollider->SetHeightLocalY(height);
}


void Quad::BoxColliderComponent::SetDepthLocalZ(float depth)
{
	mBoxCollider->SetDepthLocalZ(depth);
}

void Quad::BoxColliderComponent::SetPositionLocal(const DirectX::XMFLOAT3& pos)
{
	mBoxCollider->SetPositionLocal(pos);

}
Quad::BoxCollider* Quad::BoxColliderComponent::GetCollider() const
{
	const Transform& transform = GetTransform();

	if (transform.GetDirtyFlag())
		mBoxCollider->Update(GetTransformWorldMatrixF());

	return mBoxCollider;
}
