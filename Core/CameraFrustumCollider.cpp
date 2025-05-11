#include "Collision/CameraFrustumCollider.h"
#include"Object/Camera/Camera.h"
#include"ResourceManager/MeshManager.h"
#include"Collision/ColliderGenerator.h"
#include"Asset/Mesh/Mesh.h"


//시야공간을 기준으로 되어있다 항상 

// UPDATE에세 시야행렬의 역행렬을 곱해주는것
Quad::CameraFrustumCollider::~CameraFrustumCollider()
{
	//프러스텀 충돌체의 메시는 mesh manager에 들어가지않는다.
	//따라서 mesh삭제처리를 여기서한다.
	delete mMesh;

}

void Quad::CameraFrustumCollider::Initialize(Camera* camera,Mesh * frustumMesh )
{
	const DirectX::XMFLOAT4X4 & projMatrix =camera->GetProjMatrix();
	DirectX::BoundingFrustum frustum;
	DirectX::BoundingFrustum::CreateFromMatrix(frustum, DirectX::XMLoadFloat4x4(&projMatrix));

	SetDestObject(camera);
	SetMesh(frustumMesh);
	FrustumCollider::Initialize(frustumMesh, frustum);



}

void Quad::CameraFrustumCollider::UpdateCameraVolume(const DirectX::XMFLOAT4X4& projMatrix)
{
	DirectX::BoundingFrustum::CreateFromMatrix(mBoundingFrustumOrigin, DirectX::XMLoadFloat4x4(&projMatrix));
	
	ColliderGenerator::UploadFrustumVertexVector((FrustumCamera*)GetDestObject(), mMesh->GetVertexUploadBuffer());
	UpdateCollider();
}


Quad::CameraFrustumCollider::CameraFrustumCollider()
	:mMesh(nullptr)
{

}


void Quad::CameraFrustumCollider::SetMesh(Mesh* mesh)
{
	mMesh = mesh;
}

void Quad::CameraFrustumCollider::UpdateCollider()
{

	const DirectX::XMFLOAT4X4 & worldMatrixF 	= GetTransform().GetWorldMatrix();
	mBoundingFrustumOrigin.Transform(mBoundingFrustum, DirectX::XMLoadFloat4x4(&worldMatrixF));

}
