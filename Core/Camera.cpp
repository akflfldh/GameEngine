#include "Object/Camera/Camera.h"
#include"Utility/Utility.h"
#include"Core/System.h"
#include"Core/WindowResizeEvent.h"
#include"Collision/Collider.h"
#include"Collision/ColliderGenerator.h"
#include"Collision/CameraBoxCollider.h"
#include"Collision/CameraFrustumCollider.h"

#include"Parser/JsonParser.h"
#include"Core/CameraEventComponentFactory.h"
namespace Quad
{
	Camera::Camera(const std::string& name, ECameraType cameraType)
		:Object(name,EObjectType::eCamera),mType(cameraType), mNear(1.0f), mFar(100000.0f)
	{

		//카메라 종류에따라 콜라이더 적절히 얻고 설정

		//GetModel()->AddComponernt(EComponentType::eColliderComponent);




	}

	Camera::~Camera()
	{
		System* system = GetSystem();
		if (system != nullptr)
			system->RemoveEntityInEventTable("WindowResizeEvent", this);

	}



	void Camera::Initialize()
	{
		System* system = GetSystem();
		if (system != nullptr)
			system->RegisterEntityToEvent("WindowResizeEvent", this);


		//if (mType == ECameraType::ePerspectiveCamera)
		//{
		//	DirectX::XMStoreFloat4x4(&mProjMatrix, DirectX::XMMatrixPerspectiveFovLH(mFovY, mAspect, mNear, mFar));
		//	mViewFrustumCollider = ColliderGenerator::CreateCameraFrustumCollider(this);
		//	mViewFrustumCollider->SetSystem(system);
		//}
		//else
		//{
		//	//DirectX::XMStoreFloat4x4(&mProjMatrix, DirectX::XMMatrixOrthographicLH(mViewWidth, mViewHeight, mNear, mFar));
		//	DirectX::XMStoreFloat4x4(&mProjMatrix, DirectX::XMMatrixOrthographicLH(clientWidth, clientHeight, mNear, mFar));
		//	mOrthogonalViewCollider = ColliderGenerator::CreateBoxCameraCollider(this);
		//	mOrthogonalViewCollider->SetSystem(system);

		//}
		mTransform.Initialize();//위험하다.
		//UpdateViewMatrix();



	}

	void Camera::DefaultCreatingInitialize()
	{
	}



	void Camera::Update(float deltaTime)
	{
		Object::Update(deltaTime);
		UpdateViewMatrix();
	}

	void Camera::OnEvent(Event* event)
	{
		const std::string& eventName = event->GetName();

		if (eventName == "WindowResizeEvent")
		{
			HandleWindowResizeEvent(event);
		}

		for (auto& eventComponent : mCameraEventComponentVector)
		{
			eventComponent.second->OnEvent(event);
		}

	}

	void Camera::EndUpdate(float deltaTime)
	{


	}

	//void Camera::UpdateCollider(float deltaTime)
	//{
	//	////카메라에대한 콜라이더는 시야공간을 기준으로하니깐 
	//	////시야공간의 역행렬을 곱해서 월드행렬로 가야하는거지.
	//	//DirectX::XMFLOAT4X4  viewMatrix = GetViewMatrix();
	//	//DirectX::XMMATRIX invViewMatrix = DirectX::XMMatrixInverse(nullptr,DirectX::XMLoadFloat4x4(&viewMatrix));

	//	//if (mType == ECameraType::eOrthogonalCamera)
	//	//{
	//	//	mOrthogonalViewCollider->Update(deltaTime, invViewMatrix);
	//	//	return;
	//	//}

	//	//mViewFrustumCollider->Update(deltaTime, invViewMatrix);
	//}

	//void Camera::SetAspect(float aspect)
	//{
	//	mAspect = aspect;
	//	if (mType == ECameraType::ePerspectiveCamera)
	//	{
	//		DirectX::XMStoreFloat4x4(&mProjMatrix, DirectX::XMMatrixPerspectiveFovLH(mFovY, mAspect, mNear, mFar));
	//		mViewFrustumCollider->UpdateCameraVolume(mProjMatrix);
	//	}
	//	else
	//	{
	//		//DirectX::XMStoreFloat4x4(&mProjMatrix, DirectX::XMMatrixOrthographicLH(mViewWidth, mViewHeight, mNear, mFar));
	//		DirectX::XMStoreFloat4x4(&mProjMatrix, DirectX::XMMatrixOrthographicLH(mViewWidth, mViewHeight, mNear, mFar));
	//		
	//		CameraBoxCollider* cameraBoxCollider = (CameraBoxCollider*)mOrthogonalViewCollider;
	//		cameraBoxCollider->ResizeCameraSize(mViewWidth, mViewHeight,mNear,mFar);

	//	}

	//}

	void Camera::SetNearPlane(float nearZ)
	{
		mNear = nearZ;
		//EvaluateProjMatrix();

	
	}

	void Camera::SetFarPlane(float farZ)
	{
		mFar = farZ;
		//EvaluateProjMatrix();
	
	}


	ECameraType Camera::GetCameraType() const
	{
		return mType;
	}

	DirectX::XMFLOAT4X4 Camera::GetProjMatrix() const
	{
		return mProjMatrix;
	}

	DirectX::XMFLOAT4X4 Camera::GetViewMatrix() const
	{
		return mViewMatrix;
	}

	DirectX::XMFLOAT4X4 Camera::GetViewProjMatrix() const
	{
		DirectX::XMFLOAT4X4 viewProj;
		DirectX::XMFLOAT4X4 view = GetViewMatrix();
		DirectX::XMFLOAT4X4 proj = GetProjMatrix();

		DirectX::XMStoreFloat4x4(&viewProj, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&view), DirectX::XMLoadFloat4x4(&proj)));
		return viewProj;
		//return Utility::GetIdentityMatrixF();

	}




	float Camera::GetNearPlane() const
	{
		return mNear;
	}

	float Camera::GetFarPlane() const
	{
		return mFar;
	}


	void Camera::SetPosition(float x, float y, float z)
	{
		GetTransform().SetPositionLocal({ x,y,z });

	}

	void Camera::SetPositionWorld(float x, float y, float z)
	{
		GetTransform().SetPositionWorld({ x,y,z });

	}

	void Camera::SetPosition(const DirectX::XMFLOAT3& pos)
	{

		GetTransform().SetPositionLocal(pos);
	}

	void Camera::SetPositionWorld(const DirectX::XMFLOAT3& pos)
	{
		GetTransform().SetPositionWorld(pos);

	}


	void Camera::UpdateViewMatrix()
	{
		Transform& transform = GetTransform();
		DirectX::XMFLOAT3 lookWorld = transform.GetLookWorld();
		DirectX::XMFLOAT3 rightWorld = transform.GetRightWorld();
		DirectX::XMFLOAT3 upWorld = transform.GetUpWorld();
		DirectX::XMFLOAT3 posWorld = transform.GetPositionWorld();


		DirectX::XMFLOAT4X4 viewMatrix;

		viewMatrix._11 = rightWorld.x;
		viewMatrix._21 = rightWorld.y;
		viewMatrix._31 = rightWorld.z;


		viewMatrix._12 = upWorld.x;
		viewMatrix._22 = upWorld.y;
		viewMatrix._32 = upWorld.z;

		viewMatrix._13 = lookWorld.x;
		viewMatrix._23 = lookWorld.y;
		viewMatrix._33 = lookWorld.z;

		viewMatrix._41 = -1 * (rightWorld.x * posWorld.x + rightWorld.y * posWorld.y + rightWorld.z * posWorld.z);
		viewMatrix._42 = -1 * (upWorld.x * posWorld.x + upWorld.y * posWorld.y + upWorld.z * posWorld.z);
		viewMatrix._43 = -1 * (lookWorld.x * posWorld.x + lookWorld.y * posWorld.y + lookWorld.z * posWorld.z);

		viewMatrix._14 = 0.0f;
		viewMatrix._24 = 0.0f;
		viewMatrix._34 = 0.0f;
		viewMatrix._44 = 1.0f;

		mViewMatrix = viewMatrix;
	}

	/*void Camera::HandleWindowResizeEvent(Event* pEvent)
	{

		WindowResizeEvent* windowResizeEvent = (WindowResizeEvent*)pEvent;
		SetViewWidthAndHeight(windowResizeEvent->GetWindowWidth(), windowResizeEvent->GetWindowHeight());

	}*/

	void Camera::SetProjectMatrix(const DirectX::XMFLOAT4X4& projMatrix)
	{
		mProjMatrix = projMatrix;
	}



	//bool Camera::InnerDetectCollsion(Collider* colliderA, Collider* colliderB)
	//{

	//	return true;
	//}
	//bool Camera::InnerDetectRayHit(Collider* collder, const Ray& ray)
	//{
	//	return true;
	//}

	void Camera::Serialize()
	{
		Object::Serialize();

		JsonParser::Write("Camera_Near", mNear);
		JsonParser::Write("Camera_Far", mFar);



	}

	void Camera::DeSerialize()
	{
		Object::DeSerialize();

		JsonParser::Read("Camera_Near", mNear);
		JsonParser::Read("Camera_Far", mFar);

	}

	bool Camera::RegisterCameraEventComponent(int id, ICameraEventComponent* cameraEventComponent)
	{
		if (cameraEventComponent == nullptr)
			return false;

		auto it = std::find_if(mCameraEventComponentVector.begin(), mCameraEventComponentVector.end(), [id](const std::pair<int, ICameraEventComponent*>& element) {


			if (element.first == id)
				return true;
			return false;
			});

		if (it != mCameraEventComponentVector.end())
			return false;


		mCameraEventComponentVector.push_back({ id,cameraEventComponent });
		return true;
	}

	ICameraEventComponent * Camera::RemoveCameraEventComponent(int id)
	{
		auto it = std::find_if(mCameraEventComponentVector.begin(), mCameraEventComponentVector.end(), [id](const std::pair<int, ICameraEventComponent*>& element) {


			if (element.first == id)
				return true;
			return false;
			});


		if (it == mCameraEventComponentVector.end())
			return nullptr;


		ICameraEventComponent* component = it->second;
		mCameraEventComponentVector.erase(it);

		return component;



	}


















}