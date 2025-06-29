#include "Collision/OrientedCollider.h"

#include<Parser/JsonParser.h>

Quad::OrientedBoxCollider::OrientedBoxCollider()
	:Collider(EColliderType::eOrientedBox)
{
}

Quad::OrientedBoxCollider::~OrientedBoxCollider()
{
}

void Quad::OrientedBoxCollider::Update(const DirectX::XMFLOAT4X4& transformMatrix)
{
}

void Quad::OrientedBoxCollider::Update(const DirectX::XMMATRIX& transformMatrix)
{
}

void Quad::OrientedBoxCollider::SetPositionLocal(const DirectX::XMFLOAT3& pos)
{
	mBoundingOrientedBoxOrigin.Center = pos;

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



const DirectX::BoundingOrientedBox& Quad::OrientedBoxCollider::GetBoundingVolume() const
{
	return mBoundingOrientedBox;
	// TODO: 여기에 return 문을 삽입합니다.
}
