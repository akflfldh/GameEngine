#pragma once



#include"Predeclare.h"
#include"Object/Entity.h"
#include"GizmoHandle.h"
#include"GizmoRotationHandle.h"
#include"GizmoScaleHandle.h"
#include"EditorMacro.h"
namespace Quad
{
	class EditorMap;

	enum class EGizmoMode
	{
		eTranslation=0,
		eRotation,
		eScaling

	};

	class Gizmo:public Entity
	{
	public:
		Gizmo(const std::string & name="");
		~Gizmo() = default;

		virtual void Initialize() override;

		static Gizmo* Create(EditorMap* map, int mapLayerIndex = 1);




		void Update(float deltaTime) override;
		virtual void OnEvent(Event* event) override;

		CLASSNAME(Gizmo)


		//virtual void Update(float deltaTime) override;
		//virtual void OnEvent(Event* event) override;
		//virtual void UpdateCollider(float deltaTime);
		void SetDestObject(Object* object);

		void Move(const DirectX::XMFLOAT3& shift);
		void Rotate(const DirectX::XMFLOAT3 & yawPitchRoll);
		void Scale(const DirectX::XMFLOAT3& scale);
			bool GetChildSelectState()const;
		bool CheckChildHandleSelect(Object* object) const;
		
		EGizmoMode GetGizmoMode() const;
		void SetGizmoMode(EGizmoMode mode);

		void OnGizmo();
		void OffGizmo();




	private:
		void HandleKeyDownEvent(Event* event);

		void InitCreating(Map* map);

		 



		GizmoHandle* mHandleX;
		GizmoHandle* mHandleY;
		GizmoHandle* mHandleZ;

		GizmoRotationHandle* mRotationHandleX;	//(ex x축을 기준으로 회전한다라는의미)
		GizmoRotationHandle* mRotationHandleY;
		GizmoRotationHandle* mRotationHandleZ;

		GizmoScaleHandle* mScaleHandleX;
		GizmoScaleHandle* mScaleHandleY;
		GizmoScaleHandle* mScaleHandleZ;


		EGizmoMode mMode;







		Object* mDestObject;

		float mDefaultSize;		//near plane위에있을떄의 기즈모의디폴트스케일크기값.
		//카메라와의 거리가멀어질수록 기즈모의 크기를 증가시킨다.

	};
	REGISTEREDITORCLASS(Gizmo)
}
