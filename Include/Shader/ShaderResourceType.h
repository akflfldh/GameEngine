#pragma once


#include"header.h"

//struct EntityConstant
//{
//	DirectX::XMFLOAT4X4 mWorld;
//};
//
//
//struct PassConstant
//{
//	DirectX::XMFLOAT4X4 mViewProj;
//};


enum class EShaderResourceDescriptorType
{
	eShaderResourceCbv= 0,
	eShaderResourceSrv,
	eShaderResourceUav,
	eShaderResourceRtv
};
