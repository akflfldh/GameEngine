#include "Core/Ray.h"

namespace Quad
{
	Ray::Ray()
		:mOrigin({ 0,0,0 }), mDir({ 0,0,0 })
	{

	}

	Ray::Ray(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 dir)
		: mOrigin(origin), mDir(dir)
	{
	}

	Ray::~Ray()
	{
	}

	void Ray::SetOrigin(DirectX::XMFLOAT3 origin)
	{
		mOrigin = origin;
	}

	void Ray::SetDir(DirectX::XMFLOAT3 dir)
	{
		mDir = dir;
	}

	DirectX::XMFLOAT3 Ray::GetOrigin() const
	{
		return mOrigin;
	}

	DirectX::XMFLOAT3 Ray::GetDir() const
	{
		return mDir;
	}

	bool Ray::IsInWorldSpace() const
	{
		return mWorldSpaceFlag;
	}

	void Ray::SetWorldSpaceFlag(bool flag)
	{
		mWorldSpaceFlag = flag;
	}

	void Ray::SetParameterT(float t)
	{
		mParameterT = t;
	}

	float Ray::GetParameterT() const
	{
		return mParameterT;
	}


}