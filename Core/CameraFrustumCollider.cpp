#include "Collision/CameraFrustumCollider.h"
#include"Object/Camera/Camera.h"
#include"ResourceManager/MeshManager.h"
#include"Collision/ColliderGenerator.h"
#include"Asset/Mesh/Mesh.h"


//시야공간을 기준으로 되어있다 항상 

Quad::CameraFrustumCollider::CameraFrustumCollider()
{
}

Quad::CameraFrustumCollider::~CameraFrustumCollider()
{


}

void Quad::CameraFrustumCollider::SetProjMatrix(const DirectX::XMFLOAT4X4& projMatrix)
{

	DirectX::BoundingFrustum::CreateFromMatrix(mBoundingFrustumOrigin, DirectX::XMLoadFloat4x4(&projMatrix));
	mBoundingFrustum = mBoundingFrustumOrigin;
}


void Quad::CameraFrustumCollider::UpdateCameraVolume(const DirectX::XMFLOAT4X4& projMatrix)
{
	DirectX::BoundingFrustum::CreateFromMatrix(mBoundingFrustumOrigin, DirectX::XMLoadFloat4x4(&projMatrix));
	mBoundingFrustum = mBoundingFrustumOrigin;
	//ColliderGenerator::UploadFrustumVertexVector((FrustumCamera*)GetDestObject(), mMesh->GetVertexUploadBuffer());
	//UpdateCollider();
}

