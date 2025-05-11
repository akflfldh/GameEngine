#pragma once

#include"Object/Camera/FrustumCamera.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB DefaultMoveFrustumCamera:public FrustumCamera
	{
	public:
		DefaultMoveFrustumCamera();
		virtual ~DefaultMoveFrustumCamera();
		

		virtual void Initialize() override;
		static DefaultMoveFrustumCamera* Create(Map* map, int mapLayerIndex, float fovY, float aspect);


		CLASSNAME(DefaultMoveFrustumCamera)
		CLASSSIZE(DefaultMoveFrustumCamera)
		virtual void Start() override;

	protected:
		void InitCreating(float fovY, float aspect);


	private:



	};
	REGISTERCLASS(DefaultMoveFrustumCamera)



}
