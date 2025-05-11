#pragma once




#include"Predeclare.h"


#include"SphereCollider.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB PointLightCollider:SphereCollider
	{
	public:
		PointLightCollider() = default;
		~PointLightCollider() = default;

		void ReSizeRadius(float radius);
	private:


	};

}
