#pragma once

#include"header.h"
#include"Predeclare.h"
#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB CameraHelper
	{

	public:

		CameraHelper() = delete;
		~CameraHelper() = default;

		static void CalculateRay(DirectX::XMFLOAT2 screenPos, D3D12_VIEWPORT viewPort,
			const Camera& camera, bool optionToWolrd, Ray& oRay);

		static void CalculateScreenToViewPos(DirectX::XMFLOAT2 screenPos, D3D12_VIEWPORT viewPort,
			const Camera& camera, bool optionToWolrd, DirectX::XMFLOAT3& oPos);


	private:






	};


}