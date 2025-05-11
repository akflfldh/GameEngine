#pragma once

#include"Predeclare.h"
#include"Entity.h"
#include"EditorMacro.h"
namespace Quad
{
	
	class GizmoRotationHandle:public Entity
	{
	public:
		GizmoRotationHandle(const std::string & name="");
		~GizmoRotationHandle() = default;


		virtual void Initialize() override;

		static GizmoRotationHandle* Create(Map * map, int mapLayerIndex, EAxisType axisType, Gizmo * gizmo);

		void SetColor(const DirectX::XMFLOAT3 & color);
		void SetScale(float scale);


		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event * event) override;

		CLASSNAME(GizmoRotationHandle)
		EAxisType GetAxisType() const;

		virtual bool InnerDetectCollsion(Collider* colliderA, Collider* colliderB) override;
		virtual bool InnerDetectRayHit(const Ray& ray) override;


	private:
		void HandleLButtonDown(Event * pEvent);
		void HandleLButtonUp(Event * pEvent);
		void HandleHover(Event * pEvent);
		void HandleMouseMove(Event * pEvent);



	private:

		void ActiveQuadrant(int num);	//1,2,3,4(ex 1사분면)
		void InitCreating(EAxisType axisType, Gizmo* gizmo);

	private:
	
		EAxisType mAxisType;
		Gizmo* mGizmo;
		int mActiveQuadrantIndex;
		float mInnerCircleRadius;
		float mOuterCircleRadius;

		float mDefaultInnerCircleRadius;
		float mDefaultOuterCircleRadius;

		DirectX::XMFLOAT3 mDefaultColor;
		DirectX::XMFLOAT3 mHoverColor;

	};
	REGISTEREDITORCLASS(GizmoRotationHandle)
}
