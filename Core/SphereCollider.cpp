#include "Collision/SphereCollider.h"
#include"Parser/JsonParser.h"

Quad::SphereCollider::SphereCollider()
	:Collider(EColliderType::eSphere)
{
	mBoundingSphere.Center = { 0,0,0 };
	mBoundingSphere.Radius = 1.0f;


}

Quad::SphereCollider::~SphereCollider()
{
}


void Quad::SphereCollider::Update(const DirectX::XMFLOAT4X4& transformMatrix)
{
}

void Quad::SphereCollider::Update(const DirectX::XMMATRIX& transformMatrix)
{
}

void Quad::SphereCollider::SetRadiusLocal(float radius)
{
	mBoundingSphereOrigin.Radius = radius;

}

void Quad::SphereCollider::SetPositionLocal(const DirectX::XMFLOAT3& pos)
{
	mBoundingSphereOrigin.Center = pos;

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




const DirectX::BoundingSphere& Quad::SphereCollider::GetBoundingVolume() const
{

	return	mBoundingSphere;
	// TODO: 여기에 return 문을 삽입합니다.
}
