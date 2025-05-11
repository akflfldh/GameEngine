#pragma once

#include"Object/Camera/Camera.h"
#include"Object/Macro.h"
#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB OrthogoanlCamera:public Camera
	{
	public:
		OrthogoanlCamera(const std::string & name ="");
		~OrthogoanlCamera();
		CLASSNAME(OrthogoanlCamera)
		CLASSSIZE(OrthogoanlCamera)
		virtual void Initialize();

		static OrthogoanlCamera* Create(Map* map, int mapLayerIndex, UINT width, UINT height);


		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event* event) override;



		virtual void SetNearPlane(float nearZ) override;
		virtual void SetFarPlane(float farZ) override;

		void SetViewWidth(float viewWidth);		//직교투영카메라일때만유효하다.
		void SetViewHeight(float viewHeight);	// 동일
		void SetViewWidthAndHeight(float viewWidth, float viewHeight);


		float GetViewWidth()const;
		float GetViewHeight()const;



	/*	virtual void SetPosition(float x, float y, float z) override;
		virtual void SetPositionWorld(float x, float y, float z) override;
		virtual void SetPosition(const DirectX::XMFLOAT3& pos) override;
		virtual void SetPositionWorld(const DirectX::XMFLOAT3& pos) override;*/



	/*	virtual const Collider* GetCollider() const override;
		virtual Collider* GetCollider() override;*/

		virtual void EvaluateProjMatrix() override;

		virtual void Serialize() override;
		virtual void DeSerialize() override;


		virtual Collider* GetViewCollider() const override;



	protected:
		void InitCreating(UINT width, UINT height);

		void ReCalculateViewColliderPosition();
		virtual void HandleWindowResizeEvent(Event* pEvent) override;

	private:
		

		float mViewWidth;
		float mViewHeight;
		CameraBoxCollider* mOrthogonalViewCollider;

	};
	REGISTERCLASS(OrthogoanlCamera)
}

