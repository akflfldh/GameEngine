#include "Collision/FrustumCollider.h"
#include"Parser/JsonParser.h"
void Quad::FrustumCollider::Initialize(Mesh * mesh,
	float nearPlane, float farPlane, float rightSlope, float leftSlope,
	float topSlope, float bottomSlope, const DirectX::XMFLOAT3 &origin ,const DirectX::XMFLOAT4 & orientation)
{
	Collider::Initialize(mesh);

	mBoundingFrustum.Origin = origin;
	mBoundingFrustum.Near = nearPlane;
	mBoundingFrustum.Far = farPlane;
	mBoundingFrustum.RightSlope = rightSlope;
	mBoundingFrustum.LeftSlope =leftSlope;
	mBoundingFrustum.TopSlope = topSlope;
	mBoundingFrustum.BottomSlope =bottomSlope;
	mBoundingFrustum.Orientation = orientation;

	mBoundingFrustumOrigin = mBoundingFrustum;

}

void Quad::FrustumCollider::Initialize(Mesh * mesh , const DirectX::BoundingFrustum& boundingFrustum)
{
	Collider::Initialize(mesh);
	mBoundingFrustum = boundingFrustum;
	mBoundingFrustumOrigin = boundingFrustum;
}

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

void Quad::FrustumCollider::OnEvent(Event* event)
{
}

void Quad::FrustumCollider::Update(float deltaTime)
{
}

void Quad::FrustumCollider::Update(float deltaTime, const DirectX::XMFLOAT4X4& transformMatrix)
{
	mBoundingFrustumOrigin.Transform(mBoundingFrustum, DirectX::XMLoadFloat4x4(&transformMatrix));

	GetTransform().SetTransformWorld(transformMatrix);

}

void Quad::FrustumCollider::Update(float deltaTime, const DirectX::XMMATRIX& transformMatrix)
{
	mBoundingFrustumOrigin.Transform(mBoundingFrustum, transformMatrix);
	DirectX::XMFLOAT4X4 transformMatirxF;
	DirectX::XMStoreFloat4x4(&transformMatirxF, transformMatrix);

	GetTransform().SetTransformWorld(transformMatirxF);


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

void Quad::FrustumCollider::SetMesh(Mesh* mesh)
{
}
