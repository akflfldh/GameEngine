#include "Component/ColliderBaseComponent.h"
#include"Collision/ColliderGenerator.h"
#include"Collision/Collider.h"
#include"Map/Map.h"
#include"Core/SceneElement.h"

#include"Parser/JsonParser.h"


Quad::ColliderBaseComponent::ColliderBaseComponent()
{
}

Quad::ColliderBaseComponent::~ColliderBaseComponent()
{



}


void  Quad::ColliderBaseComponent::SetCollider(BaseCollider* collider)
{
	


}


void Quad::ColliderBaseComponent::Serialize(const std::string& tag)
{
	
	//mCollider->Serialize();




}

void Quad::ColliderBaseComponent::DeSerialize(const std::string& tag)
{
	
	//mCollider = ColliderGenerator::CreateDefaultCollider(className);
//	mCollider->DeSerialize();

}

