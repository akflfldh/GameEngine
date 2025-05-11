#include "GameEntity.h"
#include"ResourceManager/MeshManager.h"



Quad::GameEntity::GameEntity()
{
}

Quad::GameEntity::~GameEntity()
{
}

void Quad::GameEntity::InitCreating()
{
	Entity::InitCreating();

	MeshComponent* meshComponent = static_cast<MeshComponent*>(GetModel()->GetComponent(EComponentType::eMeshComponent));
	meshComponent->SetMesh(MeshManager::GetMesh(reinterpret_cast<const char*>(u8"Asset/메시/Cube")));

}
