#include "Component/UiColliderComponent.h"
#include"Object/UiEntity.h"

#include"Object/Line//Line.h"
#include"Object/Line//Spline.h"
#include"Collision/RectCollider.h"
#include"Collision/SplineCollider.h"
#include"Collision/ColliderGenerator.h"
Quad::UiColliderComponent::UiColliderComponent(Object* uiEntity)
	:ModelBaseComponent(uiEntity)
{
	switch (uiEntity->GetObjectType())
	{
	case EObjectType::eUiEntity:
	{
		mCollider = ColliderGenerator::CreateRectCollider();
		mCollider->SetDestObject(uiEntity);
	}
	break;

	case EObjectType::eSpline:
	{
		mCollider = ColliderGenerator::CreateSplineCollider();
		mCollider->SetDestObject(uiEntity);
		break;

	}





	};
}

Quad::UiColliderComponent::~UiColliderComponent()
{


}

void Quad::UiColliderComponent::Update(float deltaTime)
{
	



}

void Quad::UiColliderComponent::Serialize(const std::string& tag)
{



}

void Quad::UiColliderComponent::DeSerialize(const std::string& tag)
{



}

Quad::UiCollider* Quad::UiColliderComponent::GetCollider() const
{

	return mCollider;
}
