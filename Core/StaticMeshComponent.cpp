#include "Component/StaticMeshComponent.h"
#include"Asset/Mesh/Mesh.h"
#include"Core/SceneElement.h"
#include"Component/ColliderBaseComponent.h"
#include"ResourceManager/MeshManager.h"

#include"Object/Object.h"

#include"Collision/ColliderGenerator.h"

#include"Parser/JsonParser.h"


Quad::StaticMeshComponent::StaticMeshComponent()
{
	SetCollider(ColliderGenerator::CreateBoxCollider());
}

Quad::StaticMeshComponent::~StaticMeshComponent()
{
}


void Quad::StaticMeshComponent::Start()
{


}

void Quad::StaticMeshComponent::Update(float deltaTime)
{

}

void Quad::StaticMeshComponent::SetMesh(Mesh* mesh)
{
	if (mesh || mesh->GetMeshType() != EMeshType::eStaticMesh)
		return;

	//기존에설정된 메시가 있다면, 먼저 그와관련된 reset작업수행

	IMeshComponent::SetMesh(mesh);
	if (mesh != nullptr)
	{
		static_cast<BoxCollider*>(GetCollider())->SetMinMaxVertexLocal(mesh->GetMinMaxPoint());
	}


}



void Quad::StaticMeshComponent::Serialize(const std::string& tag)
{
	IMeshComponent::Serialize(tag);

}

void Quad::StaticMeshComponent::DeSerialize(const std::string& tag)
{
	IMeshComponent::DeSerialize(tag);

}


