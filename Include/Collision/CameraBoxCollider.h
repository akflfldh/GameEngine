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

	//	void ResizeCameraSize(float width, float height, float nearPlane, float farPlane);

	private:
		CameraBoxCollider()=default;



	};


}
