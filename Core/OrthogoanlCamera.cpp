#include "Object/Camera/OrthogoanlCamera.h"
#include"Collision/CameraBoxCollider.h"
#include"Collision/ColliderGenerator.h"
#include"Component/ColliderComponent.h"
#include"Map/Map.h"

Quad::OrthogoanlCamera::OrthogoanlCamera(const std::string& name)
	:Camera(name,ECameraType::eOrthogonalCamera),mViewWidth(1000),mViewHeight(1000)
{

	//mOrthogonalViewCollider = ColliderGenerator::CreateBoxCameraCollider(this);
	//GetModel()->GetColliderComponent()->SetCollider(ColliderGenerator::CreateBoxCameraCollider(this));
}

Quad::OrthogoanlCamera::~OrthogoanlCamera()
{
}

void Quad::OrthogoanlCamera::Initialize()
{
	Camera::Initialize();
	mOrthogonalViewCollider = ColliderGenerator::CreateBoxCameraCollider(this);


	GetTransform().SetTransformChangeEventFlag(true);
	GetTransform().RegisterTransformChangeCallback([collider= mOrthogonalViewCollider , dstCamera =  this ](){

		const Transform& cameraTransform = dstCamera->GetTransform();

		//카메라와 동일한방향
		const DirectX::XMFLOAT4 & orientation  = cameraTransform.GetQuaternionWorld();
		collider->GetTransform().SetQuaternionWorld(orientation);



		DirectX::XMFLOAT3  posWorld = dstCamera->GetTransform().GetPositionWorld();
		//카메라의 위치에서 look 방향으로 이동조정을 해준다
		DirectX::XMFLOAT3 lookWorld = cameraTransform.GetLookWorld();
		float toCenterLength = dstCamera->GetNearPlane() + (dstCamera->GetNearPlane() + dstCamera->GetFarPlane()) / 2;
		
		DirectX::XMFLOAT3 center;
		DirectX::XMStoreFloat3(&center, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&posWorld),DirectX::XMVectorScale(DirectX::XMLoadFloat3(&lookWorld), toCenterLength)));

		collider->GetTransform().SetPositionLocal(center);
		collider->UpdateCollider();
		
		});


}

Quad::OrthogoanlCamera* Quad::OrthogoanlCamera::Create(Map* map, int mapLayerIndex, UINT width, UINT height)
{
	OrthogoanlCamera* ob = static_cast<OrthogoanlCamera*>(map->CreateObject("OrthogoanlCamera", mapLayerIndex));
	ob->Initialize();
	ob->InitCreating(width, height);
	return ob;
}

void Quad::OrthogoanlCamera::Update(float deltaTime)
{
	Camera::Update(deltaTime);

}

void Quad::OrthogoanlCamera::OnEvent(Event* event)
{
	Camera::OnEvent(event);
}

void Quad::OrthogoanlCamera::SetNearPlane(float nearZ)
{

	Camera::SetNearPlane(nearZ);

	Transform& cameraTransform = GetTransform();
	Transform & viewColliderTransform	=	mOrthogonalViewCollider->GetTransform();

	viewColliderTransform.SetScaleLocal({ mViewWidth,mViewHeight,{GetFarPlane() - nearZ} });

	ReCalculateViewColliderPosition();

	EvaluateProjMatrix();


	mOrthogonalViewCollider->UpdateCollider();
}

void Quad::OrthogoanlCamera::SetFarPlane(float farZ)
{

	Camera::SetFarPlane(farZ);

	Transform& cameraTransform = GetTransform();
	Transform& viewColliderTransform = mOrthogonalViewCollider->GetTransform();

	viewColliderTransform.SetScaleLocal({ mViewWidth,mViewHeight,{farZ - GetNearPlane()}});

	ReCalculateViewColliderPosition();

	EvaluateProjMatrix();


	mOrthogonalViewCollider->UpdateCollider();
}

void Quad::OrthogoanlCamera::SetViewWidth(float viewWidth)
{
	mViewWidth = viewWidth;
	

	mOrthogonalViewCollider->GetTransform().SetScaleLocal({ mViewWidth,mViewHeight,GetFarPlane() - GetNearPlane() });

	EvaluateProjMatrix();



	mOrthogonalViewCollider->UpdateCollider();

}

void Quad::OrthogoanlCamera::SetViewHeight(float viewHeight)
{
	mViewHeight = viewHeight;
	
	mOrthogonalViewCollider->GetTransform().SetScaleLocal({ mViewWidth,mViewHeight,GetFarPlane() - GetNearPlane() });

	EvaluateProjMatrix();


	mOrthogonalViewCollider->UpdateCollider();
}

void Quad::OrthogoanlCamera::SetViewWidthAndHeight(float viewWidth, float viewHeight)
{
	mViewWidth = viewWidth;
	mViewHeight = viewHeight;

	mOrthogonalViewCollider->GetTransform().SetScaleLocal({ mViewWidth,mViewHeight,GetFarPlane() - GetNearPlane() });

	EvaluateProjMatrix();


	mOrthogonalViewCollider->UpdateCollider();

}

float Quad::OrthogoanlCamera::GetViewWidth() const
{
	return mViewWidth;
}

float Quad::OrthogoanlCamera::GetViewHeight() const
{
	return mViewHeight;
}

//void Quad::OrthogoanlCamera::SetPosition(float x, float y, float z)
//{
//	OrthogoanlCamera::SetPosition(x, y, z);
//
//}
//
//void Quad::OrthogoanlCamera::SetPositionWorld(float x, float y, float z)
//{
//	OrthogoanlCamera::SetPositionWorld({ x, y, z });
//}
//
//void Quad::OrthogoanlCamera::SetPosition(const DirectX::XMFLOAT3& pos)
//{
//
//	Camera::SetPosition(pos);
//	//mOrthogonalViewCollider->SetPosition(pos);
//
//}
//
//void Quad::OrthogoanlCamera::SetPositionWorld(const DirectX::XMFLOAT3& pos)
//{
//
//	Camera::SetPositionWorld(pos);
//
//
//}

//const Quad::Collider* Quad::OrthogoanlCamera::GetCollider() const
//{
//	return mOrthogonalViewCollider;
//}
//
//Quad::Collider* Quad::OrthogoanlCamera::GetCollider()
//{
//	return mOrthogonalViewCollider;
//}

void Quad::OrthogoanlCamera::EvaluateProjMatrix()
{
	DirectX::XMFLOAT4X4 projMatrix;
	DirectX::XMStoreFloat4x4(&projMatrix,	DirectX::XMMatrixOrthographicLH(mViewWidth, mViewHeight, GetNearPlane(), GetFarPlane()));
	SetProjectMatrix(projMatrix);


	//CameraBoxCollider* collideer = static_cast<CameraBoxCollider*>(GetModel()->GetColliderComponent()->GetCollider());

	//collideer->ResizeCameraSize(mViewWidth, mViewHeight, GetNearPlane(), GetFarPlane());

}

void Quad::OrthogoanlCamera::Serialize()
{
	Camera::Serialize();
	JsonParser::Write("OrthogoanlCamera_ViewWidth", mViewWidth);
	JsonParser::Write("OrthogoanlCamera_ViewHeight", mViewHeight);

}

void Quad::OrthogoanlCamera::DeSerialize()
{
	Camera::DeSerialize();
	JsonParser::Read("OrthogoanlCamera_ViewWidth", mViewWidth);
	JsonParser::Read("OrthogoanlCamera_ViewHeight", mViewHeight);

	EvaluateProjMatrix();
	UpdateViewMatrix();

}

Quad::Collider* Quad::OrthogoanlCamera::GetViewCollider() const
{
	return mOrthogonalViewCollider;
}

void Quad::OrthogoanlCamera::InitCreating(UINT width, UINT height)
{
	mViewWidth = width;
	mViewHeight = height;
	//mOrthogonalViewCollider->SetWidthX(width);
	//mOrthogonalViewCollider->SetHeightY(height);
	//mOrthogonalViewCollider->SetDepthZ(GetFarPlane());

//	DirectX::XMFLOAT3 cameraPos = GetTransform().GetPositionWorld();
	//cameraPos.z+=
// 	mOrthogonalViewCollider->SetPosition({				GetNearPlane()})

	Transform& cameraTransform = GetTransform();

	mOrthogonalViewCollider->GetTransform().SetScaleLocal({ (float)width,(float)height,GetFarPlane() - GetNearPlane() });
	mOrthogonalViewCollider->GetTransform().SetQuaternionLocal(GetTransform().GetQuaternionLocal());

	ReCalculateViewColliderPosition();

	//GetModel()->GetColliderComponent()->GetCollider()->SetSystem(GetSystem());

	EvaluateProjMatrix();
	UpdateViewMatrix();

	mOrthogonalViewCollider->UpdateCollider();

}

void Quad::OrthogoanlCamera::ReCalculateViewColliderPosition()
{
	Transform& cameraTransform = GetTransform();

	DirectX::XMFLOAT3 pos = cameraTransform.GetPositionWorld();
	DirectX::XMFLOAT3 look = cameraTransform.GetLookWorld();
	float toCenterDistance = GetNearPlane() + (GetFarPlane() - GetNearPlane()) / 2;
	DirectX::XMStoreFloat3(&pos, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&pos), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&look), toCenterDistance)));

	mOrthogonalViewCollider->GetTransform().SetPositionLocal(pos);
}

void Quad::OrthogoanlCamera::HandleWindowResizeEvent(Event* pEvent)
{








}
