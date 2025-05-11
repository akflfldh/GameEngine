#include "Collision/UiCollider.h"

#include"Object/Object.h"
Quad::UiCollider::UiCollider(EUiColliderType colliderType)
	:mDestObject(nullptr),mColliderType(colliderType)
{
}

Quad::UiCollider::~UiCollider()
{

}

void Quad::UiCollider::SetDestObject(Object* object)
{
	mDestObject = object;
	if (mDestObject->GetName() == "OrthogoanlCamera2")
	{
		int a = 2;
	}
}


Quad::Object* Quad::UiCollider::GetDestObject() const
{
	return mDestObject;
}

Quad::EUiColliderType Quad::UiCollider::GetColliderType() const
{
	return mColliderType;
}

