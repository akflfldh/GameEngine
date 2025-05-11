#pragma once

#include"Utility/Singleton.h"


#include"Allocator/DestructingPoolAllocator.h"


#include"BezierCurve.h"
#include"HermiteCurve.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB  CurveFactory:public Singleton<CurveFactory>
	{
	public:
		CurveFactory();
		~CurveFactory() = default;
		
		static BezierCurve* CreateBezierCurve();
		static HermiteCurve* CreateHermiteCurve();

		static void ReleaseCurve(Curve* curve);


	private:
		DestructingPoolAllocator<BezierCurve> mBezierCuveAllocator;
		DestructingPoolAllocator<HermiteCurve> mHermiteCuveAllocator;



	};

}
