#pragma once


#include<CoreMath.h>
enum class EMeshType :int
{
	eStaticMesh = 0,
	eSknningMesh,
	eLine
};

struct UIVertex
{
	CoreMath::Vector3 mPos;
	CoreMath::Vector2 mTex;
};
