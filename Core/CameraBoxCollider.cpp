#include "Collision/CameraBoxCollider.h"

void Quad::CameraBoxCollider::ResizeCameraSize(float width, float height,float nearPlane, float farPlane)
{
	//mBoundingBoxOrigin.Extents.x = width / 2.0f;
	//mBoundingBoxOrigin.Extents.y = height / 2.0f;
	DirectX::XMFLOAT4X4 defaultMatrix;
	float sizeZ = farPlane - nearPlane;
	DirectX::XMVECTOR center = DirectX::XMVectorSet(0, 0, (farPlane - nearPlane) / 2.0f, 1.0f);

	DirectX::XMMATRIX translation =	DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorSet(0, 0, (farPlane - nearPlane) / 2.0f, 1.0f));

	DirectX::XMStoreFloat4x4(&defaultMatrix, 
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixScalingFromVector(
				DirectX::XMVectorSet(width, height, sizeZ, 0.0f)),translation));
	//카메라 시야공간에서는 원점에항상존재한다.
	//이동성분은 없다.
	SetDefaultMatrix(defaultMatrix);


}

void Quad::CameraBoxCollider::SetMesh(Mesh* mesh)
{
}

void Quad::CameraBoxCollider::UpdateCollider()
{

	const DirectX::XMFLOAT4X4& worldMatrixF = GetTransform().GetWorldMatrix();
	DirectX::XMMATRIX worldMatrix =DirectX::XMLoadFloat4x4(&worldMatrixF);

	mBoundingBoxOrigin.Transform(mBoundingBox, worldMatrix);

}
