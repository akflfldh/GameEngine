#pragma once


#include"Collision/FrustumCollider.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB CameraFrustumCollider:public FrustumCollider
	{

		friend class ColliderGenerator;
	
	public:
		~CameraFrustumCollider();

		
		void Initialize(Camera* camera, Mesh* frustumMesh);

		CLASSNAME(CameraFrustumCollider)
		CLASSSIZE(CameraFrustumCollider)
		void UpdateCameraVolume(const DirectX::XMFLOAT4X4& projMatrix);


		virtual void SetMesh(Mesh* mesh) override;

		//카메라의 위치나,방향이 바뀌엇을때 호출하여 내부 콜라이더를 업데이트한다.
		void UpdateCollider();


	protected:

	

	private:
		CameraFrustumCollider();
		
		Mesh* mMesh;


	};

}
