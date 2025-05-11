#include "Core/DefaultMoveFrustumCamera.h"


#include"MoveComponent.h"
#include"LookComponent.h"

#include"Map/Map.h"

Quad::DefaultMoveFrustumCamera::DefaultMoveFrustumCamera()	
{
	ModelBaseComponent * component =	GetModel()->GetComponent(EComponentType::eColliderComponent);

	if (component != nullptr)
		int a = 2;
}

Quad::DefaultMoveFrustumCamera::~DefaultMoveFrustumCamera()
{
	ModelBaseComponent* component = GetModel()->GetComponent(EComponentType::eColliderComponent);

	if (component != nullptr)
		int a = 2;
}

void Quad::DefaultMoveFrustumCamera::Initialize()
{
	FrustumCamera::Initialize();


	AddComponent(new MoveComponent(this));
	AddComponent(new LookComponent(this));

	MoveComponent* moveComponent = static_cast<MoveComponent*>(GetComponent(ComponentIDGenerator::GetID< MoveComponent>()));

	moveComponent->SetSpeed(2.0);
}

Quad::DefaultMoveFrustumCamera* Quad::DefaultMoveFrustumCamera::Create(Map* map, int mapLayerIndex, float fovY, float aspect)
{
	DefaultMoveFrustumCamera* ob = static_cast<DefaultMoveFrustumCamera*>(map->CreateObject("DefaultMoveFrustumCamera", mapLayerIndex));


	//ob->Initialize();
	ob->InitCreating(fovY, aspect);

	return ob;


}

void Quad::DefaultMoveFrustumCamera::Start()
{
	FrustumCamera::Start();

}

void Quad::DefaultMoveFrustumCamera::InitCreating(float fovY, float aspect)
{

	FrustumCamera::InitCreating(fovY, aspect);
	//FrustumCamera::Start();


	//AddComponent(new MoveComponent(this));
	//AddComponent(new LookComponent(this));

	//MoveComponent* moveComponent = static_cast<MoveComponent*>(GetComponent(ComponentIDGenerator::GetID< MoveComponent>()));

	//moveComponent->SetSpeed(2.0);
}
