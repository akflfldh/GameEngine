#include "Collision/BoxCollider.h"
#include<Parser/JsonParser.h>



Quad::BoxCollider::BoxCollider()
	:Collider(EColliderType::eBox)
{

	SetMinMaxVertexLocal({ -0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,0.5f });
	mBoundingBoxOrigin = mBoundingBox;


}

const DirectX::BoundingBox& Quad::BoxCollider::GetBoundingVolume() const
{
	
	
	return mBoundingBox;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::BoxCollider::~BoxCollider()
{



}


void Quad::BoxCollider::Update(const DirectX::XMFLOAT4X4& transformMatrix)
{

	mBoundingBoxOrigin.Transform(mBoundingBox, DirectX::XMLoadFloat4x4(&transformMatrix));

}

void Quad::BoxCollider::Update(const DirectX::XMMATRIX& transformMatrix)
{
	mBoundingBoxOrigin.Transform(mBoundingBox, transformMatrix);
}

void Quad::BoxCollider::Serialize()
{
	Collider::Serialize();
	JsonParser::Write("BoxCollider_Center", mBoundingBox.Center);
	JsonParser::Write("BoxCollider_Extents", mBoundingBox.Extents);
	JsonParser::Write("BoxCollider_OrignalCenter", mBoundingBoxOrigin.Center);
	JsonParser::Write("BoxCollider_OrignalExtents", mBoundingBoxOrigin.Extents);

}

void Quad::BoxCollider::DeSerialize()
{
	Collider::DeSerialize();

	JsonParser::Read("BoxCollider_Center", mBoundingBox.Center);
	JsonParser::Read("BoxCollider_Extents", mBoundingBox.Extents);
	JsonParser::Read("BoxCollider_OrignalCenter", mBoundingBoxOrigin.Center);
	JsonParser::Read("BoxCollider_OrignalExtents", mBoundingBoxOrigin.Extents);


}


void Quad::BoxCollider::SetMinMaxVertexLocal(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max)
{
	DirectX::BoundingBox::CreateFromPoints(mBoundingBoxOrigin, DirectX::XMLoadFloat3(&min), DirectX::XMLoadFloat3(&max));
}

void Quad::BoxCollider::SetMinMaxVertexLocal(const std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>& minMax)
{
	SetMinMaxVertexLocal(minMax.first, minMax.second);
}

void Quad::BoxCollider::SetWidthLocalX(float width)
{
	if (width <= 0)
		return;


	mBoundingBoxOrigin.Extents.x = width / 2;
}

void Quad::BoxCollider::SetHeightLocalY(float height)
{
	if (height <= 0)
		return;


	mBoundingBoxOrigin.Extents.y = height / 2;



}

void Quad::BoxCollider::SetDepthLocalZ(float depth)
{

	if (depth <= 0)
		return;

	mBoundingBoxOrigin.Extents.z = depth / 2;

}

void Quad::BoxCollider::SetPositionLocal(const DirectX::XMFLOAT3& pos)
{
	mBoundingBoxOrigin.Center = pos;

}
