#pragma once


#include"Collision/BoxCollider.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB CameraBoxCollider:public BoxCollider
	{

		friend class ColliderGenerator;
		friend class CollisionHelper;
	public:
		~CameraBoxCollider() = default;

		CLASSNAME(CameraBoxCollider)
		CLASSSIZE(CameraBoxCollider)
		//view width, view height가변하면 original bounding box가 변해야한다.

		void ResizeCameraSize(float width, float height, float nearPlane, float farPlane);

		virtual void SetMesh(Mesh* mesh) override;


		//카메라의 시야를나타내는 충돌체로서 충돌체의 transform을 내부 collider에 적용하는 함수
		// CameraBoxCollider는 다른 update말고 이 함수만호출하면된다.
		void UpdateCollider();


	private:
		CameraBoxCollider()=default;



	};


}
