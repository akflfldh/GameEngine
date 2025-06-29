#pragma once


#include"Collision/FrustumCollider.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB CameraFrustumCollider:public FrustumCollider
	{

		friend class ColliderGenerator;
	

	public:
		CameraFrustumCollider();
		virtual ~CameraFrustumCollider();


		void SetProjMatrix(const DirectX::XMFLOAT4X4 & projMatrix);

		CLASSNAME(CameraFrustumCollider)
		CLASSSIZE(CameraFrustumCollider)
		void UpdateCameraVolume(const DirectX::XMFLOAT4X4& projMatrix);
	protected:

	
	private:
	


	};

}
