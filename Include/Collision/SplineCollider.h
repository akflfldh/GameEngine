#pragma once


#include"Collision/UiCollider.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB SplineCollider:public UiCollider
	{
	public:
		SplineCollider();
		virtual ~SplineCollider();

		virtual void Update(float deltaTime) override;


	private:

	};

}

