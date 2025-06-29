#include "RectColliderComponent.h"

Quad::RectColliderComponent::RectColliderComponent()
{
}

Quad::RectColliderComponent::~RectColliderComponent()
{
}

void Quad::RectColliderComponent::Update()
{
}

void Quad::RectColliderComponent::Serialize(const std::string& tag)
{
}

void Quad::RectColliderComponent::DeSerialize(const std::string& tag)
{
}

Quad::RectCollider* Quad::RectColliderComponent::GetCollider() const
{
	const Transform &  transform = GetTransform();


	if(transform.GetDirtyFlag())
		mRectCollider->Update(GetTransformWorldMatrixF());


	return mRectCollider;
}
