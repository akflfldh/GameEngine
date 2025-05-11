#pragma once

#include<DirectXMath.h>
namespace Quad
{

	struct BoneAnimSRT
	{
		DirectX::XMFLOAT3 mScale;
		DirectX::XMFLOAT4 mRotation;	//quaternion
		DirectX::XMFLOAT3 mTranslation;
	};


}