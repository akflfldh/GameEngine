#include "Collision/OrientedCollider.h"

Quad::OrientedBoxCollider::OrientedBoxCollider()
	:Collider(EColliderType::eOrientedBox)
{
}

Quad::OrientedBoxCollider::~OrientedBoxCollider()
{
}

void Quad::OrientedBoxCollider::OnEvent(Event* event)
{




}

void Quad::OrientedBoxCollider::Update(float deltaTime)
{



}

void Quad::OrientedBoxCollider::Update(float deltaTime, const DirectX::XMFLOAT4X4& transformMatrix)
{



}

void Quad::OrientedBoxCollider::Update(float deltaTime, const DirectX::XMMATRIX& transformMatrix)
{



}

void Quad::OrientedBoxCollider::Serialize()
{
	Collider::Serialize();
	JsonParser::Write("OrientedBoxCollider_Center", mBoundingOrientedBox.Center);
	JsonParser::Write("OrientedBoxCollider_Extents", mBoundingOrientedBox.Extents);
	JsonParser::Write("OrientedBoxCollider_Orientation", mBoundingOrientedBox.Orientation);

}

void Quad::OrientedBoxCollider::DeSerialize()
{

	Collider::DeSerialize();
	JsonParser::Read("OrientedBoxCollider_Center", mBoundingOrientedBox.Center);
	JsonParser::Read("OrientedBoxCollider_Extents", mBoundingOrientedBox.Extents);
	JsonParser::Read("OrientedBoxCollider_Orientation", mBoundingOrientedBox.Orientation);

}

void Quad::OrientedBoxCollider::SetMesh(Mesh* mesh)
{
}


void Quad::OrientedBoxCollider::Initialize(Mesh * mesh ,const DirectX::XMFLOAT3 & origin, const DirectX::XMFLOAT3 & extents)
{
	Collider::Initialize(mesh );


	mBoundingOrientedBox.Center = origin;
	mBoundingOrientedBox.Extents =  extents;
	//mBoundingOrientedBox.Orientation 방향을 나타내는단위쿼터니언


}

const DirectX::BoundingOrientedBox& Quad::OrientedBoxCollider::GetBoundingVolume() const
{
	return mBoundingOrientedBox;
	// TODO: 여기에 return 문을 삽입합니다.
}
