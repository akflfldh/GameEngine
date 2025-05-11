#pragma once


#include"Object/Object.h"
#include"Object/ObjectType.h"


#include"Core/CoreDllExport.h"

#include<vector>
#include"ICameraEventComponent.h"
#include"ComponentIDGenerator.h"
namespace Quad
{

	
	class CORE_API_LIB Camera :public Object
	{
	public:
		Camera(const std::string & name ,ECameraType cameraType);
		virtual ~Camera() = 0;

		virtual void Initialize() override;
		virtual void DefaultCreatingInitialize()override;

		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event* event) override;
		virtual void EndUpdate(float deltaTime) override;
	//	virtual void UpdateCollider(float deltaTime) = 0;


		//CLASSNAME(Camera)
		CLASSSIZE(Camera)

		virtual void SetNearPlane(float nearZ);
		virtual void SetFarPlane(float farZ) ;
		
		ECameraType GetCameraType() const;

		DirectX::XMFLOAT4X4 GetProjMatrix() const;
		DirectX::XMFLOAT4X4 GetViewMatrix() const;
		DirectX::XMFLOAT4X4 GetViewProjMatrix() const;


		float GetNearPlane() const;
		float GetFarPlane()const;
	

		virtual void SetPosition(float x, float y, float z);
		virtual void SetPositionWorld(float x, float y, float z);
		virtual void SetPosition(const DirectX::XMFLOAT3 & pos);
		virtual void SetPositionWorld(const DirectX::XMFLOAT3 & pos);

		//virtual const Collider* GetCollider() const = 0;
		//virtual Collider* GetCollider() = 0;
	
		//이 메서드는 아무것도하지않고 인터페이스역할만 한다.
	//	virtual bool InnerDetectCollsion(Collider* colliderA, Collider* colliderB) override;
	//	virtual bool InnerDetectRayHit(Collider* collder, const Ray& ray) override;


		//setter메서드내에서 호출된다.(setnear,setfar)
		virtual void EvaluateProjMatrix() = 0;

		virtual void Serialize() override;
		virtual void DeSerialize() override;

		virtual Collider* GetViewCollider() const = 0;

	public:

		//직접사용하지말고 RegisterCameraEventComponentHelperMethod , 
		//ReleaseCameraEventComponentHelperMethod 를 사용할것
		bool RegisterCameraEventComponent(int id ,ICameraEventComponent* cameraEventComponent);
		ICameraEventComponent* RemoveCameraEventComponent(int id);

	protected:


		void UpdateViewMatrix();
		virtual void HandleWindowResizeEvent(Event * pEvent) = 0;
		void SetProjectMatrix(const DirectX::XMFLOAT4X4 & projMatrix);
			

	private:
		//위치 방향은 Transform속성에있고
		// 
		//프러스텀을 정의하는 속성들

		ECameraType mType;

		float mNear;
		float mFar;

		DirectX::XMFLOAT4X4 mProjMatrix;
		DirectX::XMFLOAT4X4 mViewMatrix;


		//CameraFrustumCollider* mViewFrustumCollider;//직교투영일경우 직육면체일것이다.
		//BoxCollider* mOrthogonalViewCollider;
		//

		//<id, component>
		std::vector<std::pair<int,ICameraEventComponent*>> mCameraEventComponentVector;



	};


	template<typename ComponentType>
	bool RegisterCameraEventComponentHelperMethod(Camera* camera, ComponentType* component)
	{
		static_assert(std::is_base_of<ICameraEventComponent, ComponentType>::value, "ICameraEventComponent의 파생타입이 아닙니다.");

		return camera->RegisterCameraEventComponent(ComponentIDGenerator::GetID<ComponentType>(), component);


	}


	template<typename ComponentType>
	ICameraEventComponent* ReleaseCameraEventComponentHelperMethod(Camera* camera)
	{
		static_assert(std::is_base_of<ICameraEventComponent, ComponentType>::value, "ICameraEventComponent의 파생타입이 아닙니다.");

		return camera->RemoveCameraEventComponent(ComponentIDGenerator::GetID<ComponentType>());


	}






}