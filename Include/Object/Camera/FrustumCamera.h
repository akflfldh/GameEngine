#pragma once

#include"Object/Camera/Camera.h"
#include"Core/CoreDllExport.h"


namespace Quad
{
	class CORE_API_LIB FrustumCamera:public Camera
	{
	public:
		FrustumCamera();
		virtual ~FrustumCamera();
		CLASSNAME(FrustumCamera)
		CLASSSIZE(FrustumCamera)

		virtual void Initialize();
		
		//fovY단위는 라디안
		static FrustumCamera* Create(Map* map, int mapLayerIndex, float fovY, float aspect);


		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event* event) override;


		float GetAspect() const;
		float GetFovY() const;

		void SetFovAngleY(float radian);		//radian
		void SetAspect(float aspect);


		virtual void SetNearPlane(float nearZ) override;
		virtual void SetFarPlane(float farZ) override;

	//	virtual const Collider* GetCollider() const override;
		//virtual Collider* GetCollider() override;

		//projMatrix갱신(재계산)메서드
	
		virtual void EvaluateProjMatrix() override;

		virtual void Serialize() override;
		virtual void DeSerialize() override;


		virtual Collider* GetViewCollider() const override;

	
	protected:
		void InitCreating(float fovY, float aspect);
		virtual void HandleWindowResizeEvent(Event* pEvent) override;

	private:




		float mAspect;
		float mFovY;

		CameraFrustumCollider* mViewFrustumCollider;//직교투영일경우 직육면체일것이다.


	};
	REGISTERCLASS(FrustumCamera)
}
