#include "Object/Camera/FrustumCamera.h"
#include"Collision/ColliderGenerator.h"
#include"Collision/CameraFrustumCollider.h"
#include"Component/ColliderComponent.h"
#include"Map/Map.h"

#include"Core/WindowResizeEvent.h"

#include"Parser/JsonParser.h"
Quad::FrustumCamera::FrustumCamera()
	:Camera("", ECameraType::ePerspectiveCamera), mFovY(DirectX::XM_PI / 2), mAspect(1.0f)
{

	EvaluateProjMatrix();
	UpdateViewMatrix();
	//mViewFrustumCollider->SetCollider(ColliderGenerator::CreateCameraFrustumCollider(this));
	//mViewFrustumCollider->UpdateCameraVolume(GetProjMatrix());

}

Quad::FrustumCamera::~FrustumCamera()
{
}

void Quad::FrustumCamera::Initialize()
{

	Camera::Initialize();

	mViewFrustumCollider = ColliderGenerator::CreateCameraFrustumCollider(this);

	//mViewFrustumCollider->SetSystem(GetSystem());
	GetTransform().SetTransformChangeEventFlag(true);
	GetTransform().RegisterTransformChangeCallback([camera  = this , viewCollider = mViewFrustumCollider]() {


		Transform& cameraTransform = camera->GetTransform();
		Transform & viewColliderTransform =	 viewCollider->GetTransform();

		viewColliderTransform.SetQuaternionWorld(cameraTransform.GetQuaternionWorld());
		
		DirectX::XMFLOAT3  posWorld = cameraTransform.GetPositionWorld();
		viewColliderTransform.SetPositionWorld(posWorld);

		viewCollider->UpdateCollider();
		});

}

Quad::FrustumCamera* Quad::FrustumCamera::Create(Map* map, int mapLayerIndex, float fovY, float aspect)
{
	FrustumCamera* ob = static_cast<FrustumCamera*>(map->CreateObject("FrustumCamera", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating(fovY,aspect);



	return ob;
}

void Quad::FrustumCamera::Update(float deltaTime)
{
	Camera::Update(deltaTime);

}


void Quad::FrustumCamera::OnEvent(Event* event)
{
	Camera::OnEvent(event);

}

float Quad::FrustumCamera::GetAspect() const
{
	return mAspect;
}

float Quad::FrustumCamera::GetFovY() const
{
	return mFovY;


	
}

void Quad::FrustumCamera::SetFovAngleY(float radian)
{
	mFovY =radian;

	EvaluateProjMatrix();
	
	mViewFrustumCollider->UpdateCameraVolume(GetProjMatrix());
}

void Quad::FrustumCamera::SetAspect(float aspect)
{

	mAspect = aspect;

	EvaluateProjMatrix();
	mViewFrustumCollider->UpdateCameraVolume(GetProjMatrix());

}

void Quad::FrustumCamera::SetNearPlane(float nearZ)
{
	Camera::SetNearPlane(nearZ);



	EvaluateProjMatrix();
	mViewFrustumCollider->UpdateCameraVolume(GetProjMatrix());

}

void Quad::FrustumCamera::SetFarPlane(float farZ)
{
	Camera::SetFarPlane(farZ);

	EvaluateProjMatrix();

	mViewFrustumCollider->UpdateCameraVolume(GetProjMatrix());

}



void Quad::FrustumCamera::EvaluateProjMatrix()
{
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMStoreFloat4x4(&proj,	DirectX::XMMatrixPerspectiveFovLH(mFovY, mAspect, GetNearPlane(), GetFarPlane()));
	SetProjectMatrix(proj);




}

void Quad::FrustumCamera::Serialize()
{
	Camera::Serialize();
	JsonParser::Write("FrustumCamera_Aspect", mAspect);
	JsonParser::Write("FrustumCamera_FovY", mFovY);


}

void Quad::FrustumCamera::DeSerialize()
{
	Camera::DeSerialize();
	JsonParser::Read("FrustumCamera_Aspect", mAspect);
	JsonParser::Read("FrustumCamera_FovY", mFovY);


	EvaluateProjMatrix();
	mViewFrustumCollider->UpdateCameraVolume(GetProjMatrix());
	UpdateViewMatrix();

}

Quad::Collider* Quad::FrustumCamera::GetViewCollider() const
{
	return mViewFrustumCollider;
}

void Quad::FrustumCamera::InitCreating(float fovY, float aspect)
{

	mFovY = fovY;
	mAspect = aspect;
	EvaluateProjMatrix();
	mViewFrustumCollider->UpdateCameraVolume(GetProjMatrix());
	UpdateViewMatrix();
}

void Quad::FrustumCamera::HandleWindowResizeEvent(Event* pEvent)
{
	WindowResizeEvent* resizeEvent = static_cast<WindowResizeEvent*>(pEvent);

	float aspect =	 (float)resizeEvent->GetWindowWidth() / resizeEvent->GetWindowHeight();

	SetAspect(aspect);


}
