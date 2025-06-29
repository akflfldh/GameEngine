#include "Component/SkinningMeshComponent.h"

#include"Collision/BoxCollider.h"
#include"Collision/ColliderGenerator.h"

Quad::SkinningMeshComponent::SkinningMeshComponent()
{
	SetCollider(ColliderGenerator::CreateBoxCollider());

}

Quad::SkinningMeshComponent::~SkinningMeshComponent()
{
}

void Quad::SkinningMeshComponent::Start()
{
}

void Quad::SkinningMeshComponent::Update(float deltaTime)
{


}

void Quad::SkinningMeshComponent::SetMesh(Mesh* mesh)
{
	if (mesh  || mesh->GetMeshType() != EMeshType::eSknningMesh)
		return;

	IMeshComponent::SetMesh(mesh);
	if (mesh != nullptr)
	{
		static_cast<BoxCollider*>(GetCollider())->SetMinMaxVertexLocal(mesh->GetMinMaxPoint());
	}



}


void Quad::SkinningMeshComponent::Serialize(const std::string& tag)
{
	IMeshComponent::Serialize(tag);

}

void Quad::SkinningMeshComponent::DeSerialize(const std::string& tag)
{
	IMeshComponent::DeSerialize(tag);

}
