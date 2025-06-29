#include "Component/IMeshComponent.h"

#include"Asset/Mesh/Mesh.h"
#include"Core/SceneElement.h"
#include"Component/ColliderBaseComponent.h"
#include"ResourceManager/MeshManager.h"

#include"Object/Object.h"

#include"Collision/ColliderGenerator.h"

#include"Parser/JsonParser.h"

#include<Map/Map.h>


#include<Collision/UiCollisionWorld.h>
#include<Collision/CollisionWorld.h>


Quad::IMeshComponent::IMeshComponent()
	:mMeshPointer(nullptr), mPickingCollider(nullptr)
{


}

Quad::IMeshComponent::~IMeshComponent()
{
}



void Quad::IMeshComponent::SetMesh(Mesh* mesh)
{
	//맨초기에만 없는상태이고 외부에서 곧바로 설정할것이다.
	//그이후로는 mesh가 존재하지않는상태는없다.
	if (mesh == nullptr)
	{
		return;
	}

	//기존에설정된 메시가 있다면, 먼저 그와관련된 reset작업수행

	if (mMeshPointer != nullptr)
	{
		//subMesh 해제
		mSubMeshVector.clear();
	}


	mMeshPointer = mesh;

	const std::vector<SubMesh>& originalSubMeshVector = mesh->GetSubMeshVector();
	size_t size = originalSubMeshVector.size();
	mSubMeshVector.resize(size);
	for (size_t i = 0; i < size; ++i)
	{
		mSubMeshVector[i] = originalSubMeshVector[i];
	}


}

Quad::Mesh* Quad::IMeshComponent::GetMesh() const
{
	return mMeshPointer.GetPointer();
}



void Quad::IMeshComponent::SetActiveState(bool activeState)
{
	if (mMeshPointer == nullptr)
		activeState = false;

	//	Component::SetActiveState(activeState);

}

//std::string Model::GetName() const
//{
//	return mName;
//	// TODO: 여기에 return 문을 삽입합니다.
//}

const std::vector<Quad::ModelSubMesh>& Quad::IMeshComponent::GetSubMesh() const
{
	return mSubMeshVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

std::vector<Quad::ModelSubMesh>& Quad::IMeshComponent::GetSubMesh()
{
	return mSubMeshVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::IMeshComponent::SetMaterial(size_t subMeshIndex, Material* material)
{
	if (subMeshIndex < mSubMeshVector.size())
	{
		mSubMeshVector[subMeshIndex].mModelMaterial = *material;
	}

}

void Quad::IMeshComponent::SetTextureToAllSubMesh(Texture* texture)
{

	//잘못된것 ,이메서드는 임시적인것이라 삭제할것
	for (int i = 0; i < mSubMeshVector.size(); ++i)
	{
		mSubMeshVector[i].mModelMaterial.SetDiffuseMap(texture);
	}


}

void Quad::IMeshComponent::OnActive()
{

	Object * destObject =	GetDestObject();

	if (mPickingCollider->GetBaseColliderType() == EBaseColliderType::eUiCollider)
	{
		destObject->GetMap()->GetUiCollisionWorld()->AddCollider(static_cast<UiCollider*>(mPickingCollider));
	}
	else
	{
		destObject->GetMap()->GetCollisionWorld()->AddCollider(static_cast<Collider*>(mPickingCollider));
	}

}

void Quad::IMeshComponent::DeActive()
{
	Object* destObject = GetDestObject();

	if (mPickingCollider->GetBaseColliderType() == EBaseColliderType::eUiCollider)
	{
		destObject->GetMap()->GetUiCollisionWorld()->RemoveCollider(static_cast<UiCollider*>(mPickingCollider));
	}
	else
	{
		destObject->GetMap()->GetCollisionWorld()->RemoveCollider(static_cast<Collider*>(mPickingCollider));
	}


}

void Quad::IMeshComponent::Serialize(const std::string& tag)
{
	//	Component::Serialize(tag);

	JsonParser::Write("MeshComponent_MeshID_" + tag, mMeshPointer.GetID());


	for (int i = 0; i < mSubMeshVector.size(); ++i)
	{
		mSubMeshVector[i].Serialize(tag + std::to_string(i));
	}


}

void Quad::IMeshComponent::DeSerialize(const std::string& tag)
{

	//Component::DeSerialize(tag);

	unsigned long long meshID = 0;
	JsonParser::Read("MeshComponent_MeshID_" + tag, meshID);

	SetMesh(MeshManager::GetMesh(meshID));

	for (int i = 0; i < mSubMeshVector.size(); ++i)
	{
		mSubMeshVector[i].DeSerialize(tag + std::to_string(i));
	}


}

Quad::BaseCollider* Quad::IMeshComponent::GetCollider() const
{
	const Transform& transform = GetTransform();
	if (transform.GetDirtyFlag())
		mPickingCollider->Update(GetTransformWorldMatrixF());

	return mPickingCollider;
}

void Quad::IMeshComponent::SetCollider(BaseCollider* collider)
{
	mPickingCollider = collider;
	collider->SetParentSceneComponent(this);
}




