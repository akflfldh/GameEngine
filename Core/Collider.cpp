#include "Collision/Collider.h"
#include"Object/Entity.h"
#include"Parser/JsonParser.h"
#include"Utility/Utility.h"

Quad::Collider::Collider(EColliderType colliderType)
	:SceneElement(ESceneElementType::eCollider), mType(EObjectType::eCollider), mColliderType(colliderType), mTransform(this), mMeshComponent(nullptr), mSystem(nullptr), mMap(nullptr),
	mObject(nullptr), mDefaultMatrix(Utility::GetIdentityMatrixF())
{
	//GetModel()->AddComponernt(EComponentType::eMeshComponent);
//	mMeshComponent =static_cast<MeshComponent*>(GetModel()->GetComponent(EComponentType::eMeshComponent));
	mTransform.Initialize();
}

Quad::Collider::~Collider()
{

}



Quad::Object* Quad::Collider::GetDestObject() const
{
	return mObject;
}

void Quad::Collider::SetDestObject(Object* object)
{
	mObject = object;
	SetSystem(mObject->GetSystem());
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
	SceneElement::Serialize("Collider");
	
	JsonParser::Write("Collider_Type", (int)mColliderType);

	mTransform.Serialize("Collider");

	JsonParser::Write("Collider_DefaultMatrix", mDefaultMatrix);
}	

void Quad::Collider::DeSerialize()
{
	SceneElement::DeSerialize("Collider");

	int colliderType = 0;
	JsonParser::Read("Collider_Type", colliderType);
	mColliderType = (EColliderType)colliderType;

	mTransform.DeSerialize("Collider");

	JsonParser::Read("Collider_DefaultMatrix", mDefaultMatrix);

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

Quad::Transform& Quad::Collider::GetTransform()
{
	return mTransform;
	// TODO: 여기에 return 문을 삽입합니다.
}

const Quad::Transform& Quad::Collider::GetTransform() const
{

	return mTransform;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::MeshComponent* Quad::Collider::GetMeshComponent()
{
	return mMeshComponent;
}

const Quad::MeshComponent* Quad::Collider::GetMeshComponent() const
{
	return mMeshComponent;
}

void Quad::Collider::Initialize(Mesh* mesh)
{
	//mMeshComponent->SetMesh(mesh);
	//mMeshComponent->GetSubMesh()[0].mModelMaterial.SetEffectName("DefaultCollider.effect");
	//mModel.GetSubMesh()[0].mModelMaterial.SetEffectName("DefaultCollider.effect");

}

void Quad::Collider::SetDefaultMatrix(const DirectX::XMFLOAT4X4& defaultMatrix)
{
	mDefaultMatrix = defaultMatrix;
}

const DirectX::XMFLOAT4X4& Quad::Collider::GetDefaultMatrix() const
{
	return mDefaultMatrix;
	// TODO: 여기에 return 문을 삽입합니다.
}



