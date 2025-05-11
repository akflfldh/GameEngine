#include "Component/ColliderComponent.h"
#include"Collision/ColliderGenerator.h"
#include"Collision/Collider.h"
#include"Map/Map.h"
#include"Core/SceneElement.h"

#include"Parser/JsonParser.h"
Quad::ColliderComponent::ColliderComponent(SceneElement* sceneElement)
	:ModelBaseComponent(sceneElement),mCollider(nullptr)
{
	//collider default가 설정되어야한다.

	mCollider = ColliderGenerator::CreateDefaultCollider("BoxCollider");
	mCollider->SetDestObject((Object*)sceneElement);
}

Quad::ColliderComponent::~ColliderComponent()
{



}

void Quad::ColliderComponent::Update(float deltaTime)
{


}

void  Quad::ColliderComponent::SetCollider(Collider* collider)
{
	


	Map* map = GetDestSceneElement()->GetMap();

	//if (map == nullptr)
	//	return;			//이경우는 오직 팩토리에서 생성될때 아직 맵에 들어가기전에만 발생할것이다.

	if(mCollider !=nullptr)
	{
		ColliderGenerator::ReleaseCollider(mCollider);
		//map의 충돌공간에서 제거

		if(map!=nullptr)
		map->RemoveCollider(mCollider);

		Collider * ret = mCollider;
		mCollider = collider;
	}
	//map의 충돌공간에 추가 

	if (map != nullptr)
	map->AddCollider(mCollider);

	mCollider->SetDestObject((Object*)GetDestSceneElement());

}

void Quad::ColliderComponent::SetMesh(Mesh* mesh)
{

	mCollider->SetMesh(mesh);
}

void Quad::ColliderComponent::Serialize(const std::string& tag)
{
	JsonParser::Write("ColliderComponent_ColliderClassName", mCollider->GetClassTypeName());
	mCollider->Serialize();




}

void Quad::ColliderComponent::DeSerialize(const std::string& tag)
{
	std::string className;
	JsonParser::Read("ColliderComponent_ColliderClassName", className);

	mCollider = ColliderGenerator::CreateDefaultCollider(className);
	mCollider->DeSerialize();

}

Quad::Collider* Quad::ColliderComponent::GetCollider()
{
	return mCollider;
}

const Quad::Collider* Quad::ColliderComponent::GetCollider() const
{
	return mCollider;
}
