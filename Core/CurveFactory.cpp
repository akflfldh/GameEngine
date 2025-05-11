#include "CurveFactory.h"

Quad::CurveFactory::CurveFactory()
	:mBezierCuveAllocator(10000),mHermiteCuveAllocator(10000)
{
	mBezierCuveAllocator.Initialize();
	mHermiteCuveAllocator.Initialize();

}

Quad::BezierCurve* Quad::CurveFactory::CreateBezierCurve()
{
	auto instance = GetInstance();

	return 	instance->mBezierCuveAllocator.GetInstance();

}

Quad::HermiteCurve* Quad::CurveFactory::CreateHermiteCurve()
{

	auto instance = GetInstance();
	
	return instance->mHermiteCuveAllocator.GetInstance();

}

void Quad::CurveFactory::ReleaseCurve(Curve* curve)
{
	if (curve == nullptr)
		return;

	auto instance = GetInstance();

	BezierCurve * bezierCurve =	dynamic_cast<BezierCurve*>(curve);
	if (bezierCurve != nullptr)
	{
		instance->mBezierCuveAllocator.ReleaseInstance(bezierCurve);
	}
	else
	{
		instance->mHermiteCuveAllocator.ReleaseInstance(static_cast<HermiteCurve*>(curve));
	}



}
