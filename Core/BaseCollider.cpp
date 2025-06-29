#include "Collision/BaseCollider.h"


Quad::BaseCollider::BaseCollider(EBaseColliderType mBaseColliderType)
	:mBaseColliderType(mBaseColliderType)
{
}

Quad::BaseCollider::~BaseCollider()
{
}

void Quad::BaseCollider::SetParentSceneComponent(Quad::SceneComponent* parentComponent)
{


	mParentSceneComponent = parentComponent;
}

Quad::SceneComponent* Quad::BaseCollider::GetParentComponent() const
{
	return mParentSceneComponent;
}

Quad::EBaseColliderType Quad::BaseCollider::GetBaseColliderType() const
{
	return mBaseColliderType;
}


