#include "Component/MeshComponent.h"
#include"Asset/Mesh/Mesh.h"
#include"Core/SceneElement.h"
#include"Component/ColliderComponent.h"
#include"ResourceManager/MeshManager.h"

#include"Object/Object.h"

Quad::MeshComponent::MeshComponent(SceneElement * sceneElement)
	:ModelBaseComponent(sceneElement)
{
}

Quad::MeshComponent::~MeshComponent()
{
}

void Quad::MeshComponent::Update(float deltaTime)
{
}

void Quad::MeshComponent::SetMesh(Mesh* mesh)
{

	//기존에설정된 메시가 있다면, 먼저 그와관련된 reset작업수행

	if (mesh == nullptr)
	{
		OutputDebugString(L"SetMesh함수에서 mesh가 nullptr이 입력되었다.\n");
		return;
	}

	if (mMeshPointer != nullptr)
	{
		//subMesh 해제
		mSubMeshVector.clear();
		//mAnimationComponent.Reset();
		//animation

	}


	mMeshPointer = mesh;

	const std::vector<SubMesh>& originalSubMeshVector = mesh->GetSubMeshVector();
	size_t size = originalSubMeshVector.size();
	mSubMeshVector.resize(size);
	for (size_t i = 0; i < size; ++i)
	{
		mSubMeshVector[i] = originalSubMeshVector[i];
	}




	




	SceneElement * sceneElement =	GetDestSceneElement();
	if (sceneElement->GetSceneElementType() == ESceneElementType::eObject)
	{
		Object* object = static_cast<Object*>(sceneElement);
		if (mesh->GetMeshType() == EMeshType::eSknningMesh)
		{
			if (object->GetModel()->GetAnimationComponent() == nullptr)
			{
				object->GetModel()->AddComponernt(EComponentType::eAnimationComponent);
			}
		}
		else
		{
			if (object->GetModel()->GetAnimationComponent() != nullptr)
			{
				object->GetModel()->RemoveComponent(EComponentType::eAnimationComponent);
			}
		}
	}


	ColliderComponent* colliderComponent = static_cast<ColliderComponent*>(GetDestSceneElement()->GetModel()->GetComponent(EComponentType::eColliderComponent));
	if (colliderComponent == nullptr)
		return;
	//collider도 설정한다.
	colliderComponent->SetMesh(mesh);
	//SetCollider(ColliderGenerator::CreateBoxCollider(mesh));





}

Quad::Mesh* Quad::MeshComponent::GetMesh() const
{
	return mMeshPointer.GetPointer();
}

Quad::EMeshType Quad::MeshComponent::GetCurrentModelMeshType() const
{

	if (mMeshPointer->GetMeshType() == EMeshType::eLine)
		return EMeshType::eLine;



	AnimationComponent* animationComponent = static_cast<AnimationComponent*>(GetDestSceneElement()->GetModel()->GetComponent(EComponentType::eAnimationComponent));
	if(animationComponent!=nullptr)
	{ 
		return EMeshType::eSknningMesh;
	}
	else
	{
		return EMeshType::eStaticMesh;
	}


	//if (mAnimationComponent.GetAnimationComponentState())
	//	return EMeshType::eSknningMesh;
	//else
	//	return EMeshType::eStaticMesh;



}

//std::string Model::GetName() const
//{
//	return mName;
//	// TODO: 여기에 return 문을 삽입합니다.
//}

const std::vector<Quad::ModelSubMesh>& Quad::MeshComponent::GetSubMesh() const
{
	return mSubMeshVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

std::vector<Quad::ModelSubMesh>& Quad::MeshComponent::GetSubMesh()
{
	return mSubMeshVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::MeshComponent::SetMaterial(size_t subMeshIndex, Material* material)
{
	if (subMeshIndex < mSubMeshVector.size())
	{
		mSubMeshVector[subMeshIndex].mModelMaterial = *material;
	}

}

void Quad::MeshComponent::SetTextureToAllSubMesh(Texture* texture)
{

	//잘못된것 ,이메서드는 임시적인것이라 삭제할것
	for (int i = 0; i < mSubMeshVector.size(); ++i)
	{
		mSubMeshVector[i].mModelMaterial.SetDiffuseMap(texture);
	}


}

void Quad::MeshComponent::Serialize(const std::string& tag)
{
	JsonParser::Write("MeshComponent_MeshID_" + tag, mMeshPointer.GetID());


	for(int i=0; i<mSubMeshVector.size(); ++i)
	{
		mSubMeshVector[i].Serialize(tag+std::to_string(i));
	}






}

void Quad::MeshComponent::DeSerialize(const std::string& tag)
{
	unsigned long long meshID = 0;
	JsonParser::Read("MeshComponent_MeshID_" + tag, meshID);
	
	SetMesh(MeshManager::GetMesh(meshID));

	for (int i = 0; i < mSubMeshVector.size(); ++i)
	{
		mSubMeshVector[i].DeSerialize(tag+std::to_string(i));
	}


}
