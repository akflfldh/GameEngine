#include "Collision/BoxCollider.h"


void Quad::BoxCollider::Initialize(const DirectX::XMFLOAT3 & center, const DirectX::XMFLOAT3  &extents, Mesh* mesh)
{
	Collider::Initialize(mesh);
	mBoundingBox.Center = center;
	mBoundingBox.Extents = extents;
	mBoundingBoxOrigin = mBoundingBox;
	
}

void Quad::BoxCollider::Initialize(const DirectX::BoundingBox& boundingBox, Mesh* mesh)
{

	Collider::Initialize(mesh);
	mBoundingBox = boundingBox;
	mBoundingBoxOrigin = boundingBox;
	
	

}






Quad::BoxCollider::BoxCollider()
	:Collider(EColliderType::eBox)
{
	
	SetMinMaxVertex({ -0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,0.5f });






}

const DirectX::BoundingBox& Quad::BoxCollider::GetBoundingVolume() const
{
	
	
	return mBoundingBox;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::BoxCollider::~BoxCollider()
{



}

void Quad::BoxCollider::OnEvent(Event* event)
{



}

void Quad::BoxCollider::Update(float deltaTime)
{

	Object* destObject = GetDestObject();
	const DirectX::XMFLOAT4X4 & worldMatrixF = destObject->GetTransform().GetWorldMatrix();
	DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&worldMatrixF);
	const DirectX::XMFLOAT4X4 & defaultMatrix = GetDefaultMatrix();
	
	DirectX::XMMATRIX finalTransformMatrix = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&defaultMatrix), worldMatrix);
	DirectX::XMFLOAT4X4 finalTransformMatrixF;
	DirectX::XMStoreFloat4x4(&finalTransformMatrixF, finalTransformMatrix);

	mBoundingBoxOrigin.Transform(mBoundingBox, finalTransformMatrix);


	DirectX::XMFLOAT3 scale = { mBoundingBox.Extents.x * 2, mBoundingBox.Extents.y * 2, mBoundingBox.Extents.z * 2 };


	DirectX::XMStoreFloat4x4(&finalTransformMatrixF, DirectX::XMMatrixMultiply(DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale)), DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&mBoundingBox.Center))));

	GetTransform().SetTransformWorld(finalTransformMatrixF);



}

void Quad::BoxCollider::Update(float deltaTime, const DirectX::XMFLOAT4X4& transformMatrix)
{
	const DirectX::XMFLOAT4X4& defaultMatrix = GetDefaultMatrix();


	DirectX::XMMATRIX finalTransformMatrix =	DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&defaultMatrix), DirectX::XMLoadFloat4x4(&transformMatrix));

	DirectX::XMFLOAT4X4 finalTransformMatrixF;
	DirectX::XMStoreFloat4x4(&finalTransformMatrixF,finalTransformMatrix);

	mBoundingBoxOrigin.Transform(mBoundingBox, finalTransformMatrix);


	DirectX::XMFLOAT3 scale = { mBoundingBox.Extents.x * 2, mBoundingBox.Extents.y * 2, mBoundingBox.Extents.z * 2 };

	DirectX::XMStoreFloat4x4(&finalTransformMatrixF,DirectX::XMMatrixMultiply(DirectX::XMMatrixScalingFromVector(
		DirectX::XMLoadFloat3(&scale)),DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&mBoundingBox.Center))));


	GetTransform().SetTransformWorld(finalTransformMatrixF);
	

}

void Quad::BoxCollider::Update(float deltaTime, const DirectX::XMMATRIX& transformMatrix)
{

	const DirectX::XMFLOAT4X4& defaultMatrix = GetDefaultMatrix();

	DirectX::XMMATRIX finalTransformMatrix = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&defaultMatrix), transformMatrix);
	DirectX::XMFLOAT4X4 finalTransformMatrixF;
	DirectX::XMStoreFloat4x4(&finalTransformMatrixF, finalTransformMatrix);

	mBoundingBoxOrigin.Transform(mBoundingBox, finalTransformMatrix);

	DirectX::XMFLOAT3 scale = { mBoundingBox.Extents.x * 2, mBoundingBox.Extents.y * 2, mBoundingBox.Extents.z * 2 };
	DirectX::XMStoreFloat4x4(&finalTransformMatrixF, DirectX::XMMatrixMultiply(DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale)), DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&mBoundingBox.Center))));

	GetTransform().SetTransformWorld(finalTransformMatrixF);


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

void Quad::BoxCollider::SetMesh(Mesh* mesh)
{

	std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> minMaxPoint = mesh->GetMinMaxPoint();

	DirectX::XMFLOAT3 min = minMaxPoint.first;
	DirectX::XMFLOAT3 max = minMaxPoint.second;

//	SetMinMaxVertex({-0.5f,-0.5f,-0.5f}, { 0.5f,0.5f,0.5f });
	SetMinMaxVertex(min, max);

	//GetModel()->GetMeshComponent()->SetMesh(mesh);



//	DirectX::BoundingBox::CreateFromPoints(mBoundingBoxOrigin, DirectX::XMLoadFloat3(&min), DirectX::XMLoadFloat3(&max));







}

void Quad::BoxCollider::SetMinMaxVertex(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max)
{
	DirectX::BoundingBox::CreateFromPoints(mBoundingBoxOrigin, DirectX::XMLoadFloat3(&min), DirectX::XMLoadFloat3(&max));
}

void Quad::BoxCollider::SetWidthX(float width)
{
	if (width <= 0)
		return;


	mBoundingBoxOrigin.Extents.x = width / 2;
}

void Quad::BoxCollider::SetHeightY(float height)
{
	if (height <= 0)
		return;


	mBoundingBoxOrigin.Extents.y = height / 2;



}

void Quad::BoxCollider::SetDepthZ(float depth)
{

	if (depth <= 0)
		return;

	mBoundingBoxOrigin.Extents.z = depth / 2;

}

void Quad::BoxCollider::SetPosition(const DirectX::XMFLOAT3& pos)
{
	mBoundingBoxOrigin.Center = pos;

}
