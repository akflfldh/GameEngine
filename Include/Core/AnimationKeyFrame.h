#pragma once

#include"Predeclare.h"
#include"header.h"

namespace Quad
{

	struct AnimationKeyFrame
	{
		DirectX::XMFLOAT4 mQuaternion;
		DirectX::XMFLOAT3 mScale;
		DirectX::XMFLOAT3 mPosition;
		float mTime;
	};

}
