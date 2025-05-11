#include "Collision/SphereCollider.h"
#include"Parser/JsonParser.h"

Quad::SphereCollider::SphereCollider()
	:Collider(EColliderType::eSphere)
{
}

Quad::SphereCollider::~SphereCollider()
{
}

void Quad::SphereCollider::OnEvent(Event* event)
{
}

void Quad::SphereCollider::Update(float deltaTime)
{
}

void Quad::SphereCollider::Update(float deltaTime, const DirectX::XMFLOAT4X4& transformMatrix)
{
}

void Quad::SphereCollider::Update(float deltaTime, const DirectX::XMMATRIX& transformMatrix)
{
}

void Quad::SphereCollider::Serialize()
{
	Collider::Serialize();
	
	JsonParser::Write("SphereCollider_Center", mBoundingSphere.Center);
	JsonParser::Write("SphereCollider_Radius", mBoundingSphere.Radius);


}

void Quad::SphereCollider::DeSerialize()
{
	Collider::DeSerialize();

	JsonParser::Read("SphereCollider_Center", mBoundingSphere.Center);
	JsonParser::Read("SphereCollider_Radius", mBoundingSphere.Radius);

}

void Quad::SphereCollider::SetMesh(Mesh* mesh)
{
}

void Quad::SphereCollider::Initialize(Mesh * mesh ,const DirectX::XMFLOAT3 & center, float radius)
{
	Collider::Initialize(mesh);

	mBoundingSphere.Center = center;
	mBoundingSphere.Radius = radius;

}

const DirectX::BoundingSphere& Quad::SphereCollider::GetBoundingVolume() const
{

	return	mBoundingSphere;
	// TODO: 여기에 return 문을 삽입합니다.
}
