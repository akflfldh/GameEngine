#include "Collision/Collider.h"
#include"Object/Entity.h"
#include"Parser/JsonParser.h"
#include"Utility/Utility.h"

Quad::Collider::Collider(EColliderType colliderType)
	:BaseCollider(EBaseColliderType::eCollider),mColliderType(colliderType), mSystem(nullptr), mMap(nullptr)
{


}

Quad::Collider::~Collider()
{

}

void Quad::Collider::Initialize()
{
}


Quad::EColliderType Quad::Collider::GetColliderType() const
{
	return mColliderType;
}

void Quad::Collider::SetSystem(System* system)
{
	mSystem = system;

}

Quad::System* Quad::Collider::GetSystem() const
{
	return mSystem;
}


void Quad::Collider::Serialize()
{

	JsonParser::Write("Collider_Type", (int)mColliderType);

}	

void Quad::Collider::DeSerialize()
{

	int colliderType = 0;
	JsonParser::Read("Collider_Type", colliderType);
	mColliderType = (EColliderType)colliderType;

}

void Quad::Collider::SetMap(Map* map)
{
	mMap = map;
}

Quad::Map* Quad::Collider::GetMap() const
{
	return mMap;
}

Quad::Map* Quad::Collider::GetMap()
{
	return mMap;
}




