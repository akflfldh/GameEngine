#include "Collision/FrustumCollider.h"
#include"Parser/JsonParser.h"

const DirectX::BoundingFrustum& Quad::FrustumCollider::GetBoundingVolume() const
{
	return mBoundingFrustum;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::FrustumCollider::FrustumCollider()
	:Collider(EColliderType::eFrustum)
{
}

Quad::FrustumCollider::~FrustumCollider()
{
}


void Quad::FrustumCollider::Update(const DirectX::XMFLOAT4X4& transformMatrix)
{
	mBoundingFrustumOrigin.Transform(mBoundingFrustum, DirectX::XMLoadFloat4x4(&transformMatrix));

}

void Quad::FrustumCollider::Update(const DirectX::XMMATRIX& transformMatrix)
{
	mBoundingFrustumOrigin.Transform(mBoundingFrustum, transformMatrix);

}

void Quad::FrustumCollider::SetNearPlaneLocal(float nearPlane)
{
	mBoundingFrustumOrigin.Near = nearPlane;
}

void Quad::FrustumCollider::SetFarPlaneLocal(float farPlane)
{
	mBoundingFrustumOrigin.Far = farPlane;
}

void Quad::FrustumCollider::SetRightSlopeLocal(float rightSlope)
{
	mBoundingFrustumOrigin.RightSlope = rightSlope;
}

void Quad::FrustumCollider::SetLeftSlopeLocal(float leftSlope)
{
	mBoundingFrustumOrigin.LeftSlope = leftSlope;
}

void Quad::FrustumCollider::SetTopSlopeLocal(float topSlope)
{
	mBoundingFrustumOrigin.TopSlope = topSlope;
}

void Quad::FrustumCollider::SetBottomSlopeLocal(float bottomSlope)
{
	mBoundingFrustumOrigin.BottomSlope = bottomSlope;
}

void Quad::FrustumCollider::SetPositionLocal(const DirectX::XMFLOAT3& origin)
{
	mBoundingFrustumOrigin.Origin = origin;
}

void Quad::FrustumCollider::SetOrientationLocal(const DirectX::XMFLOAT4& orientation)
{
	mBoundingFrustumOrigin.Orientation = orientation;
}

float Quad::FrustumCollider::GetNearPlaneLocal() const
{
	return mBoundingFrustumOrigin.Near;
}

float Quad::FrustumCollider::GetFarPlaneLocal() const
{
	return mBoundingFrustumOrigin.Far;
}

float Quad::FrustumCollider::GetRightSlopeLocal() const
{
	return mBoundingFrustumOrigin.RightSlope;
}

float Quad::FrustumCollider::GetLeftSlopeLocal() const
{
	return mBoundingFrustumOrigin.LeftSlope;
}

float Quad::FrustumCollider::GetTopSlopeLocal() const
{
	return mBoundingFrustumOrigin.TopSlope;
}

float Quad::FrustumCollider::GetBottomSlopeLocal() const
{
	return mBoundingFrustumOrigin.BottomSlope;
}

const DirectX::XMFLOAT3  Quad::FrustumCollider::GetPositionLocal() const
{
	return mBoundingFrustumOrigin.Origin;
}

const DirectX::XMFLOAT4  Quad::FrustumCollider::GetOrientationLocal() const
{
	return mBoundingFrustumOrigin.Orientation;
}

void Quad::FrustumCollider::SetConfigLocal(float nearPlane, float farPlane, float rightSlope, float leftSlope, float topSlope, float bottomSlope, const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT4& orientation)
{
	mBoundingFrustumOrigin.Origin = origin;
	mBoundingFrustumOrigin.Near = nearPlane;
	mBoundingFrustumOrigin.Far = farPlane;
	mBoundingFrustumOrigin.RightSlope = rightSlope;
	mBoundingFrustumOrigin.LeftSlope = leftSlope;
	mBoundingFrustumOrigin.TopSlope = topSlope;
	mBoundingFrustumOrigin.BottomSlope = bottomSlope;
	mBoundingFrustumOrigin.Orientation = orientation;

	mBoundingFrustumOrigin = mBoundingFrustum;

}

void Quad::FrustumCollider::Serialize()
{
	Collider::Serialize();

	JsonParser::Write("FrustumCollider_Origin", mBoundingFrustum.Origin);
	JsonParser::Write("FrustumCollider_Orientation", mBoundingFrustum.Orientation);
	JsonParser::Write("FrustumCollider_RightSlope", mBoundingFrustum.RightSlope);
	JsonParser::Write("FrustumCollider_LeftSlope", mBoundingFrustum.LeftSlope);
	JsonParser::Write("FrustumCollider_TopSlope", mBoundingFrustum.TopSlope);
	JsonParser::Write("FrustumCollider_BottomSlope", mBoundingFrustum.BottomSlope);
	JsonParser::Write("FrustumCollider_Near", mBoundingFrustum.Near);
	JsonParser::Write("FrustumCollider_Far", mBoundingFrustum.Far);


	JsonParser::Write("FrustumCollider_OrignalOrigin", mBoundingFrustumOrigin.Origin);
	JsonParser::Write("FrustumCollider_OrignalOrientation", mBoundingFrustumOrigin.Orientation);
	JsonParser::Write("FrustumCollider_OrignalRightSlope", mBoundingFrustumOrigin.RightSlope);
	JsonParser::Write("FrustumCollider_OrignalLeftSlope", mBoundingFrustumOrigin.LeftSlope);
	JsonParser::Write("FrustumCollider_OrignalTopSlope", mBoundingFrustumOrigin.TopSlope);
	JsonParser::Write("FrustumCollider_OrignalBottomSlope", mBoundingFrustumOrigin.BottomSlope);
	JsonParser::Write("FrustumCollider_OrignalNear", mBoundingFrustumOrigin.Near);
	JsonParser::Write("FrustumCollider_OrignalFar", mBoundingFrustumOrigin.Far);



	




}

void Quad::FrustumCollider::DeSerialize()
{
	Collider::DeSerialize();



	JsonParser::Read("FrustumCollider_Origin", mBoundingFrustum.Origin);
	JsonParser::Read("FrustumCollider_Orientation", mBoundingFrustum.Orientation);
	JsonParser::Read("FrustumCollider_RightSlope", mBoundingFrustum.RightSlope);
	JsonParser::Read("FrustumCollider_LeftSlope", mBoundingFrustum.LeftSlope);
	JsonParser::Read("FrustumCollider_TopSlope", mBoundingFrustum.TopSlope);
	JsonParser::Read("FrustumCollider_BottomSlope", mBoundingFrustum.BottomSlope);
	JsonParser::Read("FrustumCollider_Near", mBoundingFrustum.Near);
	JsonParser::Read("FrustumCollider_Far", mBoundingFrustum.Far);


	JsonParser::Read("FrustumCollider_OrignalOrigin", mBoundingFrustumOrigin.Origin);
	JsonParser::Read("FrustumCollider_OrignalOrientation", mBoundingFrustumOrigin.Orientation);
	JsonParser::Read("FrustumCollider_OrignalRightSlope", mBoundingFrustumOrigin.RightSlope);
	JsonParser::Read("FrustumCollider_OrignalLeftSlope", mBoundingFrustumOrigin.LeftSlope);
	JsonParser::Read("FrustumCollider_OrignalTopSlope", mBoundingFrustumOrigin.TopSlope);
	JsonParser::Read("FrustumCollider_OrignalBottomSlope", mBoundingFrustumOrigin.BottomSlope);
	JsonParser::Read("FrustumCollider_OrignalNear", mBoundingFrustumOrigin.Near);
	JsonParser::Read("FrustumCollider_OrignalFar", mBoundingFrustumOrigin.Far);

}


