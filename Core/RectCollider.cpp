#include "Collision/RectCollider.h"
#include"Object/UiEntity.h"
Quad::RectCollider::RectCollider()
	:UiCollider(Quad::EUiColliderType::eRect)
{
}

Quad::RectCollider::~RectCollider()
{
}

//void Quad::RectCollider::SetDestUiEntity(UiEntity* destUiEntity)
//{
//	mDestUiEntity = destUiEntity;
//}
//
//Quad::UiEntity* Quad::RectCollider::GetDestUiEntity() const
//{
//	return mDestUiEntity;
//}

void Quad::RectCollider::Update(float deltaTime)
{
	UiEntity * destUiEntity =static_cast<UiEntity*>(GetDestObject());

	float widthLocal = destUiEntity->GetWidth();
	float heightLocal = destUiEntity->GetHeight();
	Transform & destUiEntityTransform = destUiEntity->GetTransform();

	DirectX::XMFLOAT3 posLocal = destUiEntityTransform.GetPositionLocal();
	DirectX::XMFLOAT3 topLeft = posLocal;
	//topLeft.x -= widthLocal / 2;
	//topLeft.y += heightLocal / 2;

	DirectX::XMFLOAT3 topRight = posLocal;
//	topRight.x += widthLocal / 2;
//	topRight.y += heightLocal / 2;

	DirectX::XMFLOAT3 bottomLeft = posLocal;
	//bottomLeft.x -= widthLocal / 2;
//	bottomLeft.y -= heightLocal / 2;


	DirectX::XMFLOAT3 bottomRight = posLocal;
///	bottomRight.x += widthLocal / 2;
//	bottomRight.y -= heightLocal / 2;



	topLeft = { -0.5f,0.5f,0 };
	topRight = { 0.5f,0.5f,0 };
	bottomLeft = { -0.5f,-0.5f,0 };
	bottomRight = { 0.5f,-0.5f,0 };







	DirectX::XMFLOAT4X4 worldMatrixF = destUiEntityTransform.GetWorldMatrix();
	DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&worldMatrixF);
	
	DirectX::XMStoreFloat3(&mTopLeftPoint,DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&topLeft), worldMatrix));
	DirectX::XMStoreFloat3(&mTopRightPoint,DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&topRight), worldMatrix));
	DirectX::XMStoreFloat3(&mBottomLeftPoint,DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&bottomLeft), worldMatrix));
	DirectX::XMStoreFloat3(&mBottomRightPoint,DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&bottomRight), worldMatrix));





}

const DirectX::XMFLOAT3& Quad::RectCollider::GetTopLeftPoint() const
{
	return mTopLeftPoint;
	// TODO: 여기에 return 문을 삽입합니다.
}

const DirectX::XMFLOAT3& Quad::RectCollider::GetTopRightPoint() const
{
	return mTopRightPoint;
	// TODO: 여기에 return 문을 삽입합니다.
}

const DirectX::XMFLOAT3& Quad::RectCollider::GetBottomLeftPoint() const
{
	return mBottomLeftPoint;
	// TODO: 여기에 return 문을 삽입합니다.
}

const DirectX::XMFLOAT3& Quad::RectCollider::GetBottomRightPoint() const
{
	return mBottomRightPoint;
	// TODO: 여기에 return 문을 삽입합니다.
}
